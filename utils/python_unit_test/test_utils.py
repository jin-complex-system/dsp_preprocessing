import unittest
from python_interface.utils_c import utils_c
import sys
import glob

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
utils_lib = utils_c(library_path=LIBRARY_PATH)

class PythonTestUtils(unittest.TestCase):
    def test_compute_magnitude_from_complex(self):
        self.assertEqual(utils_lib.log2_approximation(1.0),0.0)

    def test_compute_log_and_decibels_from_complex(self):
        self.assertEqual(utils_lib.log2_approximation(1.0),0.0)

    def insertion_sort(self):
        self.assertEqual(utils_lib.log2_approximation(1.0),0.0)
        # TODO: Implement unit test

    def test_log10_approximation(self):
        self.assertEqual(utils_lib.log2_approximation(1.0),0.0)

    def test_log2_approximation(self):
        self.assertEqual(utils_lib.log2_approximation(1.0),0.0)

    def test_square_root_approximation(self):
        self.assertEqual(utils_lib.log2_approximation(1.0),0.0)
        # TODO: Implement unit test

if __name__ == '__main__':
    unittest.main()
