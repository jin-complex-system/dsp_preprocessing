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
        Perform real FFT on input buffer. Converts np.float64 to np.float32 before computation

        :param self:
        :param real_input_array: 1D np.array of np.float64 or np.float32; length must be power of 2
        :return: np.array of dtype np.float32 containing real and imaginary values
        """

        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (real_input_array is not None)
        assert (len(real_input_array) >= 2 and len(real_input_array) % 2 == 0)
        assert (len(real_input_array.shape) == 1)
        assert (real_input_array.shape[0] == len(real_input_array))
        assert (real_input_array.dtype == np.float32 or real_input_array.dtype == np.float64)

        # Prepare input buffer with a deep copy
        input_buffer = np.reshape(
            real_input_array,
            shape=-1,
            copy=True,).astype(np.float32)
        assert (len(input_buffer) == len(real_input_array))
        assert (len(input_buffer) == input_buffer.shape[0])
        assert (input_buffer.dtype == np.float32)

        # Prepare buffer lengths
        n_fft = len(input_buffer)
        output_buffer_length = 2 * len(input_buffer)

        # Prepare output buffer
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)
        assert(output_buffer.shape[0] == output_buffer_length)

        # Set the return types and argument types
        self.libaudiodsp.perform_audio_dsp_rfft_direct.restype = None
        self.libaudiodsp.perform_audio_dsp_rfft_direct.argtypes = [
            np.ctypeslib.ndpointer(
                shape=[n_fft], dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=[output_buffer_length], dtype=np.float32, ndim=1),
            ctypes.c_uint32,
        ]

        # Run function
        self.libaudiodsp.perform_audio_dsp_rfft_direct(
            input_buffer,
            ctypes.c_uint16(n_fft),
            output_buffer,
            ctypes.c_uint32(output_buffer_length),
        )

        # Process the output buffer before returning
        num_actual_valid_results = int(n_fft /2 + 1) * 2
        output_buffer = output_buffer[0:num_actual_valid_results]

        return output_buffer

