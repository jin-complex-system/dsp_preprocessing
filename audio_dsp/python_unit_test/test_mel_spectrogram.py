import unittest
from python_interface.audio_dsp_c import audio_dsp_c
import sys
import glob
import os
import numpy as np
import librosa
import matplotlib.pyplot as plt

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

FREQ_TO_MEL_CONVERSION_ERROR_DELTA = 1
MEL_TO_FREQ_CONVERSION_ERROR_DELTA = 1

TEST_AUDIO_FILENAME = "test_audio.wav"
TEST_AUDIO_FILEPATH = os.path.join("python_unit_test/constants/{}".format(TEST_AUDIO_FILENAME))
TEST_MAIN_DIRECTORY = "_test_output"

class AudioDSP_MelSpectrogram_PythonTestCase(unittest.TestCase):
    def test_mel_spectrogram_to_librosa(self):
        n_ffts = [512, 1024, 2048]
        n_mels = [32, 64, 128]

        # Load the wave file as float and mono
        samples, sample_rate = librosa.load(
            path=TEST_AUDIO_FILEPATH,
            mono=True)

        for n_fft in n_ffts:
            hop_length = n_fft
            win_length = n_fft

            # Use librosa to compute mel spectrogram
            audio_stft = librosa.stft(
                samples,
                hop_length=hop_length,
                n_fft=n_fft,
                win_length=win_length
            )
            audio_stft_magnitude = librosa.amplitude_to_db(np.abs(audio_stft))
            num_frames = audio_stft_magnitude.shape[1]
            assert(audio_stft_magnitude is not None)

            for n_mel in n_mels:
                mel_spectrogram_librosa = librosa.feature.melspectrogram(
                    S=audio_stft_magnitude,
                    sr=sample_rate,
                    n_mels=n_mel
                )
                assert(mel_spectrogram_librosa.shape[0] == n_mel)

                mel_spectrogram_raw = np.zeros(
                    shape=mel_spectrogram_librosa.shape, dtype=np.float32)
                for frame_iterator in range(0, num_frames):
                    one_frame = audio_stft_magnitude[:, frame_iterator]
                    assert(one_frame.shape[0] == len(one_frame))

                    mel_spectrogram_raw[:, frame_iterator] = (
                        audio_dsp_c_lib.compute_power_spectrum_into_mel_spectrogram_raw(
                            power_spectrum_array=one_frame,
                            n_mel_uint16=n_mel,
                            n_fft_uint16=n_fft,
                            sample_rate_uint16=sample_rate,
                        ))

                # Visually compare the results
                assert (len(TEST_MAIN_DIRECTORY) > 0)
                test_output_directory = os.path.join(TEST_MAIN_DIRECTORY, "mel_spectrogram")
                os.makedirs(test_output_directory, exist_ok=True)

                common_filename = "mel_spec_fft_{}_mels_{}".format(
                    n_fft,
                    n_mel,
                )

                plt.figure()
                librosa.display.specshow(
                    mel_spectrogram_librosa,
                    cmap="magma")
                plt.axis("off")
                plt.savefig(
                    os.path.join(test_output_directory, "{}_librosa.png".format(common_filename)),
                    bbox_inches='tight',
                    pad_inches=0)
                plt.close()

                plt.figure()
                librosa.display.specshow(
                    mel_spectrogram_raw,
                    cmap="magma")
                plt.axis("off")
                plt.savefig(
                    os.path.join(test_output_directory, "{}_computed.png".format(common_filename)),
                    bbox_inches='tight',
                    pad_inches=0)
                plt.close()

if __name__ == '__main__':
    unittest.main()
