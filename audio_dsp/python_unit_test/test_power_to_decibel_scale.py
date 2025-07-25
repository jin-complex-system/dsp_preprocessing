import unittest
from python_interface.audio_dsp_c import audio_dsp_c
import sys
import glob
import os
import numpy as np
import librosa
import time

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

ERROR_DELTA_ROUNDING_ERROR = 1.0

TEST_AUDIO_FILENAME = "test_audio.wav"
TEST_AUDIO_FILEPATH = os.path.join("python_unit_test/constants/{}".format(TEST_AUDIO_FILENAME))
TEST_MAIN_DIRECTORY = "_test_output"


class AudioDSP_PowerToDecibelScale_PythonTestCase(unittest.TestCase):
    def test_convert_power_to_decibel_scale(self):
        """
        Test power to decibel conversion, and scaling

        :return:
        """
        # Constants for audio file
        n_mel = 64
        n_fft = 1024

        # Constants for scaling
        min_decibel = -80.0
        max_decibel = 0.0

        # Load the wave file as 1-D spectrogram
        samples, _ = librosa.load(
            path=TEST_AUDIO_FILEPATH,
            mono=True)
        spectrogram = librosa.feature.melspectrogram(
            y=samples,
            n_mels=n_mel,
            n_fft=n_fft,
        )
        spectrogram = np.reshape(spectrogram, shape=-1)
        spectrogram = spectrogram.astype(np.float32)
        reference_float = float(np.max(spectrogram))

        # Compute expected result from librosa
        librosa_start_time = time.time()
        log_mel_spec = librosa.power_to_db(
            S=spectrogram,
            ref=reference_float,
            amin=1e-50,
            top_db=None
        )
        log_mel_spec_norm = (log_mel_spec - min_decibel) / (max_decibel - min_decibel)
        log_mel_spec_norm = np.clip(log_mel_spec_norm, a_min=0.0, a_max=1.0)
        log_mel_spec_norm = (log_mel_spec_norm * np.iinfo(np.uint8).max).astype(np.uint8)
        librosa_end_time = time.time()
        assert (len(spectrogram) == len(log_mel_spec_norm))
        assert (
                np.min(log_mel_spec_norm) >= 0 and
                np.max(log_mel_spec_norm) <= np.iinfo(np.uint8).max)

        # Compute result
        target_start_time = time.time()
        target_result = audio_dsp_c_lib.convert_power_to_decibel_scale(
            spectrogram_array_float32=spectrogram,
            spectrogram_array_length_uint32=len(spectrogram),
            reference_power_float32=reference_float,
        )
        target_end_time = time.time()
        self.assertEqual(len(target_result), len(spectrogram))

        # Compare computation times
        target_total_time = target_end_time - target_start_time
        librosa_total_time = librosa_end_time - librosa_start_time
        self.assertLesser(
            a=target_total_time,
            b=librosa_total_time)
        print("Librosa time: {}, target time: {}".format(
            librosa_total_time, target_total_time
        ))

        # Iterate and compare results
        for iterator in range(0, len(spectrogram)):
            assert_fail_msg = (
                "\r\nWith reference float {}, Element {} of input value {} mismatch\r\n ".format(
                    reference_float,
                    iterator,
                    spectrogram[iterator],
                ))
            self.assertAlmostEqual(
                first=log_mel_spec_norm[iterator],
                second=target_result[iterator],
                delta=ERROR_DELTA_ROUNDING_ERROR,
                msg=assert_fail_msg,
            )


if __name__ == '__main__':
    unittest.main()
