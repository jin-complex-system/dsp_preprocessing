import unittest
from python_interface.statistics_c import statistics_c
import sys
import glob

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
stats_lib = statistics_c(library_path=LIBRARY_PATH)

class PythonTestUtils(unittest.TestCase):
    def test_statistics_across_frames(self):
        input_list = [0, 1, 2, 4]

        result_list = stats_lib.compute_statistics_across_frames(
            input_list,
            2,
            2,
            1.0,
        )
        # TODO: Implement unit test

        self.assertEqual(result_list, None)

if __name__ == '__main__':
    unittest.main()
