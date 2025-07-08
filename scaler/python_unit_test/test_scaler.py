import unittest

import scipy.signal
from python_interface.scaler_c import scaler_c
import sys
import glob
import numpy as np

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
scaler_c_lib = scaler_c(library_path=LIBRARY_PATH)

class Scaler_PythonTestCase(unittest.TestCase):
    def test_scaler_float(self):
        my_input = [0.0, 100.0]


if __name__ == '__main__':
    unittest.main()

