import ctypes
import numpy as np

class Struct_StatisticsC(ctypes.Structure):
    _fields_ = [
        ("max", ctypes.c_float),
        ("min", ctypes.c_float),
        ("mean", ctypes.c_float),
        ("median", ctypes.c_float),
        ("variance", ctypes.c_float),
        # ("first_der_mean", ctypes.c_float),
        # ("first_der_variance", ctypes.c_float),
    ]

class statistics_c:
    libstatistics = None

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

        assert(library_path is not None and len(library_path) > 0)
        self.libstatistics = ctypes.CDLL(library_path)

    def compute_statistics_across_frames(
            self,
            input_list_float,
            num_frames_uint32,
            bin_length_uint32,
            frame_difference_reciprocal_float,
    ):
        """
        Compute statistics from input_buffer across different frames

        :param input_list_float:
        :param num_frames_uint32:
        :param bin_length_uint32:
        :param frame_difference_reciprocal_float: non-zero, positive difference between each bin as a reciprocal
        :return:
        """

        # TODO: Implement this function

        return

    def compute_statistics_within_frame(
            self,
            input_list_float,
            num_frames_uint32,
            frame_difference_reciprocal_float,
    ):
        """
        Compute statistics from input_buffer within one frame

        :param input_list_float:
        :param num_frames_uint32:
        :param frame_difference_reciprocal_float: non-zero, positive difference between each bin as a reciprocal
        :return: float list of statistics across each frame
        """
        # Check parameters
        assert(self.libstatistics is not None)
        assert(frame_difference_reciprocal_float > 0)
        assert(num_frames_uint32 <= len(input_list_float))

        # Get the return types and argument types
        self.libstatistics.compute_statistics_within_frame.restype = None
        self.libstatistics.compute_statistics_within_frame.argtypes = [
            np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
            ctypes.c_float,
            np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
            np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
        ]

        # Prepare buffers
        NUM_STATISTICS = ctypes.sizeof(Struct_StatisticsC)

        output_buffer_length = int(NUM_STATISTICS * num_frames_uint32)
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)

        scratch_buffer_length = 2 * output_buffer_length
        scratch_buffer = np.zeros(shape=[scratch_buffer_length], dtype=np.float32)

        # Run function
        self.libstatistics.compute_statistics_within_frame(
            input_list_float,
            ctypes.c_uint32(num_frames_uint32),
            ctypes.c_float(frame_difference_reciprocal_float),
            output_buffer,
            ctypes.c_uint32(output_buffer_length),
            scratch_buffer,
            ctypes.c_uint32(scratch_buffer_length)
        )
        return output_buffer
