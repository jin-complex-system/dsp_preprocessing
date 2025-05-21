import unittest
from python_interface.utils_c import utils_c
import sys
import glob
import math
import librosa
import os
import numpy as np
import matplotlib.pyplot as plt

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
utils_lib = utils_c(library_path=LIBRARY_PATH)

TEST_MAIN_DIRECTORY = "_test_output"


class Utils_PlotErrors(unittest.TestCase):

    def test_plot_compute_decibel_from_complex(self):
        float_array = np.arange(
            start=-3000.0,
            stop=5000.0,
            step=0.01)
        error_decibel_expected = list()
        error_decibel_librosa = list()

        for float_value in float_array:
            # Computed results
            computed_decibel = utils_lib.compute_decibels_from_complex(float_value, 0.0, 0.0)

            # Expected results
            magnitude = math.sqrt(float_value * float_value + 0.0 * 0.0)
            expected_decibel = 20 * math.log10(magnitude)

            librosa_decibel = librosa.amplitude_to_db(magnitude)

            error_decibel_expected.append(abs(computed_decibel - expected_decibel))
            error_decibel_librosa.append(abs(computed_decibel - librosa_decibel))

        test_output_directory = os.path.join(TEST_MAIN_DIRECTORY, "magnitude_complex")
        os.makedirs(test_output_directory, exist_ok=True)

        plt.figure()
        plt.plot(float_array, error_decibel_expected)
        plt.savefig(
            os.path.join(test_output_directory, "error_decibel_expected.png"),
            bbox_inches='tight',
            pad_inches=0)

        plt.figure()
        plt.plot(float_array, error_decibel_librosa)
        plt.savefig(
            os.path.join(test_output_directory, "error_decibel_librosa.png"),
            bbox_inches='tight',
            pad_inches=0)


if __name__ == '__main__':
    unittest.main()
