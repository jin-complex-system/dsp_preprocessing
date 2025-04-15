import unittest
from python_interface.audio_dsp_c import audio_dsp_c
import sys
import glob
import os
import math
import numpy as np
from scipy.io import wavfile
import librosa
import matplotlib.pyplot as plt

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

TEST_AUDIO_FILENAME = "test_audio.wav"
TEST_AUDIO_FILEPATH = os.path.join("python_unit_test/constants/{}".format(TEST_AUDIO_FILENAME))
TEST_MAIN_DIRECTORY = "_test_output"

POWER_SPECTRUM_FLOAT_ERROR_DELTA = 50

class AudioDSP_PowerSpectrum_PythonTestCase(unittest.TestCase):
    def test_against_numpy_fft(self):
        n_fft_list = [
            # 512,
            1024,
            # 2048
        ]

        # Load the wave file as integers
        sample_rate, samples = wavfile.read(filename=TEST_AUDIO_FILEPATH)

        # Only use one channel (i.e mono)
        samples = samples[:, 0]

        # Load as int16
        if samples.dtype == np.int32 or samples.dtype == np.int64:
            samples = samples.astype(np.int16)
        assert (samples.dtype == np.int16)

        for n_fft in n_fft_list:
            # Constants
            NUM_SAMPLES = len(samples)
            HOP_LENGTH = n_fft
            NUM_FRAMES = math.floor(NUM_SAMPLES / HOP_LENGTH - n_fft / HOP_LENGTH) + 1
            NUM_FREQUENCY_BINS = math.floor(n_fft / 2) + 1

            # Prepare output buffer
            power_spectrum = np.zeros((NUM_FREQUENCY_BINS, NUM_FRAMES))
            expected_spectrum = np.zeros((NUM_FREQUENCY_BINS, NUM_FRAMES))

            # Compute hann window
            hann_window = audio_dsp_c_lib.hann_window_compute(
                length_uint32=n_fft,
                scaling_factor_float=float(1.0 / np.iinfo(samples.dtype).max),
            )
            self.assertEqual(len(hann_window), n_fft)

            # Iterate through the audio file
            for frame_iterator in range(NUM_FRAMES):
                begin_iterator = frame_iterator * HOP_LENGTH
                end_iterator = begin_iterator + n_fft

                if end_iterator > samples.shape[0]:
                    end_iterator = samples.shape[0]

                sample_frame = np.copy(samples[begin_iterator:end_iterator])

                # Compute expected result
                sample_frame_through_hann_window = (hann_window * sample_frame).astype(np.float32)
                assert(sample_frame_through_hann_window.dtype == np.float32)
                assert(len(sample_frame_through_hann_window) == n_fft)
                rfft_result = np.fft.rfft(
                    a=sample_frame_through_hann_window,
                    n=n_fft)
                assert (len(rfft_result) == NUM_FREQUENCY_BINS)
                expected_results = librosa.amplitude_to_db(S=np.abs(rfft_result))
                assert (len(expected_results) == NUM_FREQUENCY_BINS)

                # Run compute_power_spectrum()
                power_spectrum_results = audio_dsp_c_lib.compute_power_spectrum(
                    input_samples_array=sample_frame,
                    n_fft_uint16=n_fft,
                    window_list=hann_window
                )
                self.assertEqual(len(power_spectrum), NUM_FREQUENCY_BINS)

                # Store the results to be exported
                expected_spectrum[:, frame_iterator] = expected_results
                power_spectrum[:, frame_iterator] = power_spectrum_results

            # Compute librosa version
            samples_librosa, _ = librosa.load(
                sr=sample_rate,
                path=TEST_AUDIO_FILEPATH,
                mono=True)
            audio_stft = librosa.stft(
                samples_librosa,
                hop_length=HOP_LENGTH,
                n_fft=n_fft,
                win_length=n_fft,
                center=False,
            )

            power_spectrum_librosa = librosa.amplitude_to_db(np.abs(audio_stft))

            # Visually compare the results
            assert (expected_spectrum.shape == power_spectrum.shape)
            # assert (power_spectrum_librosa.shape == expected_spectrum.shape)
            assert (len(TEST_MAIN_DIRECTORY) > 0)
            test_output_directory = os.path.join(TEST_MAIN_DIRECTORY, "power_spectrum_numpy_fft")
            os.makedirs(test_output_directory, exist_ok=True)

            plt.figure()
            librosa.display.specshow(
                power_spectrum_librosa,
                cmap="magma")
            plt.axis("off")
            plt.savefig(
                os.path.join(test_output_directory, "librosa_power_spectrum.png"),
                bbox_inches='tight',
                pad_inches=0)
            plt.close()

            plt.figure()
            librosa.display.specshow(
                expected_spectrum,
                cmap="magma")
            plt.axis("off")
            plt.savefig(
                os.path.join(test_output_directory, "expected_power_spectrum.png"),
                bbox_inches='tight',
                pad_inches=0)
            plt.close()

            plt.figure()
            librosa.display.specshow(
                power_spectrum,
                cmap="magma")
            plt.axis("off")
            plt.savefig(
                os.path.join(test_output_directory, "computed_power_spectrum.png"),
                bbox_inches='tight',
                pad_inches=0)
            plt.close()

            # After exporting visual results, do comparison
            for frame_iterator in range(NUM_FRAMES):
                # Prepare failure message
                assert_fail_prefix_msg = "\r\nn_fft={}, hop_length={}, frame={}\r\n".format(
                    n_fft,
                    HOP_LENGTH,
                    frame_iterator)

                for bin_iterator in range(0, NUM_FREQUENCY_BINS):
                    expected_result = expected_spectrum[bin_iterator, frame_iterator]
                    computed_result = power_spectrum[bin_iterator, frame_iterator]

                    assert_fail_msg = "{} mismatch {} vs {} at bin {}".format(
                        assert_fail_prefix_msg,
                        expected_result,
                        computed_result,
                        bin_iterator
                    )

                    # TODO: Investigate why expected_result does not meet expectations
                    self.assertAlmostEqual(
                        first=expected_result,
                        second=computed_result,
                        delta=POWER_SPECTRUM_FLOAT_ERROR_DELTA,
                        msg=assert_fail_msg,
                    )

if __name__ == '__main__':
    unittest.main()
