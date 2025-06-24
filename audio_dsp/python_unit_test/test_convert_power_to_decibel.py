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

ERROR_DELTA_NUM_PLACES = 4

TEST_AUDIO_FILENAME = "test_audio.wav"
TEST_AUDIO_FILEPATH = os.path.join("python_unit_test/constants/{}".format(TEST_AUDIO_FILENAME))
TEST_MAIN_DIRECTORY = "_test_output"


class AudioDSP_ConvertPowerToDecibel_PythonTestCase(unittest.TestCase):
    def test_convert_power_to_decibel(self):
        """
        Compare function to librosa.power_to_db()
        :return:
        """
        n_mel = 64
        n_fft = 1024
        top_decibels = [
            None,
            # TODO: Investigate errors when uncomment the top decibels below
            # 20.0,
            # 80.0,
        ]

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

        for top_decibel in top_decibels:
            expected_mel_spectrogram = librosa.power_to_db(
                S=spectrogram,
                ref=reference_float,
                amin=1e-50,
                top_db=top_decibel,
            )

            computed_mel_spectrogram = audio_dsp_c_lib.convert_power_to_decibel(
                spectrogram_array_float32=spectrogram,
                spectrogram_array_length_uint16=len(spectrogram),
                reference_power_float32=reference_float,
                top_decibel_float32=top_decibel,
            )

            for iterator in range(0, len(spectrogram)):
                assert_fail_msg = (
                    "\r\nWith top decibel {} and reference float {}, Element {} of value {} mismatch\r\n ".format(
                        top_decibel,
                        reference_float,
                        iterator,
                        spectrogram[iterator],
                    ))

                self.assertAlmostEqual(
                    first=expected_mel_spectrogram[iterator],
                    second=computed_mel_spectrogram[iterator],
                    places=ERROR_DELTA_NUM_PLACES,
                    msg=assert_fail_msg,
                )


if __name__ == '__main__':
    unittest.main()
