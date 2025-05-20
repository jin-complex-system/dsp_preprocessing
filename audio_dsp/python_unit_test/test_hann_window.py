import unittest

from python_interface.audio_dsp_c import audio_dsp_c
import scipy.signal
import sys
import glob
import math
import numpy as np

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

lengths = [4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048]
scaling_factors = [1.0, 10.0, float(1.0 / np.iinfo(np.int16).max)]

HANN_WINDOW_ERROR_DELTA = 1e-5
PRECISION_ERROR_DELTA = 1e-10


def _get_hann_window_as_float(
        length,
        scale_factor=1.0,
):
    """ Compute hanning window as a float for a given length

    Favours scipy.signal.get_window() implementation rather than scipy.signal.windows.hann()

    Args:
        length - length that is greater than 1
        scale_factor - Scale hanning window if needed

    Ret:
        Return a list of the hanning window
    """

    assert(length > 1 and scale_factor > 0.0)

    hann_window_float = list()

    for iterator in range(0, length):
        hann_value = 0.5 - 0.5 * math.cos(2 * math.pi * iterator / length)
        # hann_value = 0.5 - 0.5 * math.cos(2 * math.pi * iterator / (length - 1))
        hann_value = hann_value * scale_factor

        assert(hann_value >= 0)
        hann_window_float.append(hann_value)

    return hann_window_float


class AudioDSP_HannWindow_PythonTestCase(unittest.TestCase):
    def test_against_scipy_hann_window(self):
        for length in lengths:
            for scaling_factor in scaling_factors:
                assert(length > 1 and scaling_factor > 0.0)

                assert_fail_prefix_msg = "Length: {}, scaling_factor: {}\r\n".format(
                    length,
                    scaling_factor
                )

                # Note that
                # scipy.signal.get_window() result differs from scipy.signal.windows.hann()
                # we favour librosa which uses the former
                scipy_result = scipy.signal.get_window(
                    window="hann",
                    Nx=length,
                    fftbins=True)
                scipy_result_scaled = (scipy_result * scaling_factor)
                assert (len(scipy_result_scaled) == length)
                hann_result = scipy.signal.windows.hann(
                    M=length,
                    sym=True
                )
                hann_result_scaled = (hann_result * scaling_factor)
                assert (len(hann_result_scaled) == length)

                computed_result = audio_dsp_c_lib.hann_window_compute(
                    length_uint32=length,
                    scaling_factor_float=scaling_factor,
                )
                self.assertEqual(len(computed_result), length)

                for iterator in range(0, length):
                    assert_fail_msg = "{} iterator mismatch at {}:\nscipy {}\nhann {}\ncomputed {}\n".format(
                        assert_fail_prefix_msg,
                        iterator,
                        scipy_result_scaled,
                        hann_result_scaled,
                        computed_result
                    )

                    self.assertAlmostEqual(
                        first=scipy_result_scaled[iterator],
                        second=computed_result[iterator],
                        delta=HANN_WINDOW_ERROR_DELTA,
                        msg="{}".format(assert_fail_msg))
                    # self.assertAlmostEqual(
                    #     first=hann_result_scaled[iterator],
                    #     second=computed_result[iterator],
                    #     delta=HANN_WINDOW_ERROR_DELTA,
                    #     msg="{}".format(assert_fail_msg))

    def test_against_python_hann_window(self):
        for length in lengths:
            for scaling_factor in scaling_factors:
                assert(length > 1 and scaling_factor > 0.0)

                assert_fail_prefix_msg = "Length: {}, scaling_factor: {}\r\n".format(
                    length,
                    scaling_factor
                )
                python_result = np.array(_get_hann_window_as_float(
                    length=length,
                    scale_factor=scaling_factor), dtype=np.float32)
                assert (len(python_result) == length)

                computed_result = audio_dsp_c_lib.hann_window_compute(
                    length_uint32=length,
                    scaling_factor_float=scaling_factor,
                )
                self.assertEqual(len(computed_result), length)

                for iterator in range(0, length):
                    assert_fail_msg = "{} iterator mismatch at {}:\npython {}\ncomputed {}\n".format(
                        assert_fail_prefix_msg,
                        iterator,
                        python_result,
                        computed_result
                    )

                    self.assertAlmostEqual(
                        first=python_result[iterator],
                        second=computed_result[iterator],
                        delta=PRECISION_ERROR_DELTA,
                        msg="{}".format(assert_fail_msg))


if __name__ == '__main__':
    unittest.main()
