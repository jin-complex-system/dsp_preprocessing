import unittest

import scipy.signal
from python_interface.audio_dsp_c import audio_dsp_c
import sys
import glob
import numpy as np
import librosa

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

# Delta from empirically ran tests
FFT_FLOAT64_ERROR_DELTA = 1e-4
FFT_FLOAT32_ERROR_DELTA = FFT_FLOAT64_ERROR_DELTA

n_ffts = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]


class AudioDSP_FFT_PythonTestCase(unittest.TestCase):
    def test_against_librosa_stft(self):
        for n_fft in n_ffts:
            assert (n_fft % 2 == 0)
            expected_output_complex_length = int(n_fft / 2 + 1)
            expected_output_buffer_length = expected_output_complex_length * 2

            # Prepare a random input 1D np.array()
            random_input_float64 = np.random.uniform(
                low=-1.0,
                high=1.0,
                size=n_fft, )
            np.reshape(random_input_float64, shape=-1, copy=True)
            assert (random_input_float64.shape[0] == n_fft and len(random_input_float64) == n_fft)
            assert (len(random_input_float64.shape) == 1)
            assert (random_input_float64.dtype == np.float64)

            # Get outputs from numpy and library for float32
            random_input_float32 = np.copy(random_input_float64, order='C').astype(np.float32)
            assert (len(random_input_float32) == n_fft and random_input_float32.dtype == np.float32)
            random_input_float32_with_hann = (
                    random_input_float32 *
                    scipy.signal.get_window(window="hann", Nx=n_fft, fftbins=True))

            librosa_stft_result_float32 = librosa.stft(
                y=random_input_float32,
                n_fft=n_fft,
                hop_length=n_fft,
                win_length=n_fft,
                center=False,
            )[:, 0]  # Only use the first frame
            computed_result_float32 = audio_dsp_c_lib.perform_audio_dsp_rfft(
                real_input_array=random_input_float32_with_hann,
            )
            self.assertEqual(len(librosa_stft_result_float32), expected_output_complex_length)
            self.assertEqual(len(computed_result_float32), expected_output_buffer_length)

            # Compare results
            for iterator in range(0, expected_output_complex_length):
                real_value_iterator = iterator * 2 + 0
                img_value_iterator = iterator * 2 + 1

                assert_fail_msg = "For n_fft = {}, FFT bin {} fail:\r\n ".format(n_fft, iterator)

                # Compare against librosa for float32 input
                self.assertAlmostEqual(
                    first=np.real(librosa_stft_result_float32[iterator]),
                    second=computed_result_float32[real_value_iterator],
                    delta=FFT_FLOAT32_ERROR_DELTA,
                    msg="{}real librosa float32 {} vs real lib float32 {}".format(
                        assert_fail_msg,
                        np.real(librosa_stft_result_float32[iterator]),
                        computed_result_float32[real_value_iterator])
                )
                self.assertAlmostEqual(
                    first=np.imag(librosa_stft_result_float32[iterator]),
                    second=computed_result_float32[img_value_iterator],
                    delta=FFT_FLOAT32_ERROR_DELTA,
                    msg="{}img librosa float32 {} vs img lib float32 {}".format(
                        assert_fail_msg,
                        np.imag(librosa_stft_result_float32[iterator]),
                        computed_result_float32[img_value_iterator])
                )

    def test_against_np_rfft(self):

        for n_fft in n_ffts:
            assert (n_fft % 2 == 0)
            expected_output_complex_length = int(n_fft / 2 + 1)
            expected_output_buffer_length = expected_output_complex_length * 2

            # Prepare a random input 1D np.array()
            random_input_float64 = np.random.uniform(
                low=-1.0,
                high=1.0,
                size=n_fft)
            np.reshape(random_input_float64, shape=-1, copy=True)
            assert (random_input_float64.shape[0] == n_fft and len(random_input_float64) == n_fft)
            assert (len(random_input_float64.shape) == 1)
            assert (random_input_float64.dtype == np.float64)

            # Get outputs from numpy and library for float64
            np_rfft_result_float64 = np.fft.rfft(random_input_float64)
            computed_result_float64 = audio_dsp_c_lib.perform_audio_dsp_rfft(
                real_input_array=random_input_float64,
            )
            self.assertEqual(len(np_rfft_result_float64), expected_output_complex_length)
            self.assertEqual(len(computed_result_float64), expected_output_buffer_length)

            # Get outputs from numpy and library for float32
            random_input_float32 = np.copy(random_input_float64, order='C').astype(np.float32)
            assert (len(random_input_float32) == n_fft and random_input_float32.dtype == np.float32)

            np_rfft_result_float32 = np.fft.rfft(random_input_float32)
            computed_result_float32 = audio_dsp_c_lib.perform_audio_dsp_rfft(
                real_input_array=random_input_float32,
            )
            self.assertEqual(len(np_rfft_result_float32), expected_output_complex_length)
            self.assertEqual(len(computed_result_float32), expected_output_buffer_length)

            # Compare results
            for iterator in range(0, expected_output_complex_length):
                real_value_iterator = iterator * 2 + 0
                img_value_iterator = iterator * 2 + 1

                assert_fail_msg = "For n_fft = {}, FFT bin {} fail:\r\n ".format(n_fft, iterator)

                # Conversion to float32 from float64 should not affect result
                self.assertEqual(
                    first=computed_result_float64[real_value_iterator],
                    second=computed_result_float32[real_value_iterator],
                    msg="{}real lib float64 {} vs real lib float32 {}".format(
                        assert_fail_msg,
                        computed_result_float64[real_value_iterator],
                        computed_result_float32[real_value_iterator])
                )
                self.assertEqual(
                    first=computed_result_float64[real_value_iterator],
                    second=computed_result_float32[real_value_iterator],
                    msg="{}img lib float64: {} vs img lib float32 {}".format(
                        assert_fail_msg,
                        computed_result_float64[img_value_iterator],
                        computed_result_float32[img_value_iterator])
                )

                # Compare against numpy rfft for float32 input
                self.assertAlmostEqual(
                    first=np.real(np_rfft_result_float32[iterator]),
                    second=computed_result_float32[real_value_iterator],
                    delta=FFT_FLOAT32_ERROR_DELTA,
                    msg="{}real np.rfft float32 {} vs real lib float32 {}".format(
                        assert_fail_msg,
                        np.real(np_rfft_result_float32[iterator]),
                        computed_result_float32[real_value_iterator])
                )
                self.assertAlmostEqual(
                    first=np.imag(np_rfft_result_float32[iterator]),
                    second=computed_result_float32[img_value_iterator],
                    delta=FFT_FLOAT32_ERROR_DELTA,
                    msg="{}img np.rfft float32 {} vs img lib float32 {}".format(
                        assert_fail_msg,
                        np.imag(np_rfft_result_float32[iterator]),
                        computed_result_float32[img_value_iterator])
                )

                # Compare against numpy rfft for float64 input
                self.assertAlmostEqual(
                    first=np.real(np_rfft_result_float64[iterator]),
                    second=computed_result_float32[real_value_iterator],
                    delta=FFT_FLOAT64_ERROR_DELTA,
                    msg="{}real np.rfft float64 {} vs real lib float32 {}".format(
                        assert_fail_msg,
                        np.real(np_rfft_result_float64[iterator]),
                        computed_result_float32[real_value_iterator])
                )
                self.assertAlmostEqual(
                    first=np.imag(np_rfft_result_float64[iterator]),
                    second=computed_result_float32[img_value_iterator],
                    delta=FFT_FLOAT64_ERROR_DELTA,
                    msg="{}img np.rfft float64 {} vs img lib float32 {}".format(
                        assert_fail_msg,
                        np.imag(np_rfft_result_float64[iterator]),
                        computed_result_float32[img_value_iterator])
                )


if __name__ == '__main__':
    unittest.main()
