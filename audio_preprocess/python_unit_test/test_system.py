import unittest

from python_interface.audio_preprocess_c import audio_preprocess_c
import sys
import glob

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_preprocess_c_lib = audio_preprocess_c(library_path=LIBRARY_PATH)


class AudioPreprocess_PythonTestCase(unittest.TestCase):
    def test_something(self):
        self.assertEqual(True, True)  # add assertion here


if __name__ == '__main__':
    unittest.main()
