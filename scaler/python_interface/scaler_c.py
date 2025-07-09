import ctypes
import numpy as np


class scaler_c:
    libscaler = None

    def __init__(
            self,
            library_path=""):
        if library_path != "":
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
        self.libscaler = ctypes.CDLL(library_path)

    def scale_float_buffer(
            self,
            input_buffer_float,
            top_value_float,
            bottom_value_float):
        """
        Scale input_buffer to [0.0, 1.0] as float. Clips between top_value and bottom_value

        :param input_buffer_float:
        :param top_value_float:
        :param bottom_value_float:
        :return:
        """

        # Check parameters
        assert (self.libscaler is not None)
        assert (isinstance(input_buffer_float, list) or isinstance(input_buffer_float, np.ndarray))
        assert (len(input_buffer_float) > 0)
        assert (isinstance(input_buffer_float[0], float) or isinstance(input_buffer_float[0], np.float32))
        assert (isinstance(top_value_float, float) or isinstance(top_value_float, np.float32))
        assert (isinstance(bottom_value_float, float) or isinstance(bottom_value_float, np.float32))
        assert (top_value_float > bottom_value_float)

        # Grab constants
        num_elements = len(input_buffer_float)

        # Prepare input buffer with a deep copy
        input_buffer = np.reshape(
            input_buffer_float,
            shape=-1,
            copy=True, ).astype(np.float32)
        assert (len(input_buffer) == num_elements)
        assert (len(input_buffer) == input_buffer.shape[0])
        assert (input_buffer.dtype == np.float32)

        # Prepare output buffer
        output_buffer = np.zeros(shape=[num_elements], dtype=np.float32)
        assert (len(output_buffer) == num_elements)
        assert (len(output_buffer) == input_buffer.shape[0])
        assert (output_buffer.dtype == np.float32)

        # Set the return types and argument types
        self.libscaler.scale_float_buffer.restype = None
        self.libscaler.scale_float_buffer.argtypes = [
            np.ctypeslib.ndpointer(
                shape=[num_elements], dtype=np.float32, ndim=1),
            np.ctypeslib.ndpointer(
                shape=[num_elements], dtype=np.float32, ndim=1),
            ctypes.c_uint32,
            ctypes.c_float,
            ctypes.c_float,
        ]

        self.libscaler.scale_float_buffer(
            input_buffer,
            output_buffer,
            ctypes.c_uint32(num_elements),
            ctypes.c_float(top_value_float),
            ctypes.c_float(bottom_value_float),
        )

        return output_buffer

    def scale_float_buffer_quantised(
            self,
            input_buffer_float,
            top_value_float,
            bottom_value_float):
        """
        Scale input_buffer to [0, 255] as uint8. Clips between top_value and bottom_value

        :param input_buffer_float:
        :param top_value_float:
        :param bottom_value_float:
        :return:
        """

        # Check parameters
        assert (self.libscaler is not None)
        assert (isinstance(input_buffer_float, list) or isinstance(input_buffer_float, np.ndarray))
        assert (len(input_buffer_float) > 0)
        assert (isinstance(input_buffer_float[0], float) or isinstance(input_buffer_float[0], np.float32))
        assert (isinstance(top_value_float, float) or isinstance(top_value_float, np.float32))
        assert (isinstance(bottom_value_float, float) or isinstance(bottom_value_float, np.float32))
        assert (top_value_float > bottom_value_float)

        # Grab constants
        num_elements = len(input_buffer_float)

        # Prepare input buffer with a deep copy
        input_buffer = np.reshape(
            input_buffer_float,
            shape=-1,
            copy=True, ).astype(np.float32)
        assert (len(input_buffer) == num_elements)
        assert (len(input_buffer) == input_buffer.shape[0])
        assert (input_buffer.dtype == np.float32)

        # Prepare output buffer
        output_buffer = np.zeros(shape=[num_elements], dtype=np.uint8)
        assert (len(output_buffer) == num_elements)
        assert (len(output_buffer) == input_buffer.shape[0])
        assert (output_buffer.dtype == np.uint8)

        # Set the return types and argument types
        self.libscaler.scale_float_buffer_quantised.restype = None
        self.libscaler.scale_float_buffer_quantised.argtypes = [
            np.ctypeslib.ndpointer(
                shape=[num_elements], dtype=np.float32, ndim=1),
            np.ctypeslib.ndpointer(
                shape=[num_elements], dtype=np.uint8, ndim=1),
            ctypes.c_uint32,
            ctypes.c_float,
            ctypes.c_float,
        ]

        self.libscaler.scale_float_buffer_quantised(
            input_buffer,
            output_buffer,
            ctypes.c_uint32(num_elements),
            ctypes.c_float(top_value_float),
            ctypes.c_float(bottom_value_float),
        )

        return output_buffer
