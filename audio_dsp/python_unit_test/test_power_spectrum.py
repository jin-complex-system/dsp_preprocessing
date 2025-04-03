import unittest
from python_interface.audio_dsp_c import audio_dsp_c
import sys
import glob
import math
import numpy as np

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

class AudioDSP_PowerSpectrum_PythonTestCase(unittest.TestCase):
    def test_against_numpy_fft(self):
        self.assertEqual(True, True)  # add assertion here


if __name__ == '__main__':
    unittest.main()
