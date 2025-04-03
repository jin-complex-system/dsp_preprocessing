import ctypes
import numpy as np

class audio_dsp_c:
    libaudiodsp = None

    def __init__(
            self,
            library_path=""):
        if (library_path != ""):
            self.load_library(library_path=library_path)

        def load_library(
                self,
                library_path):
            """
            Load shared library
            :param library_path:
            :return:
            """

            assert (library_path is not None and len(library_path) > 0)
            self.libaudiodsp = ctypes.CDLL(library_path)

        def perform_audio_dsp_rfft(
                self,
                real_input_array):
            """
            Perform real FFT on input buffer

            :param self:
            :param real_input_array: 1D np.array; length must be power of 2
            :return: np.array containing real and imaginary values
            """

            # Check parameters
            assert (self.libaudiodsp is not None)
            assert (real_input_array is not None)
            assert (len(real_input_array) >= 2 and len(real_input_array) % 2 == 0)

            # TODO: Implement kissFFT manipulation
