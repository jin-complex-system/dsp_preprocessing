import ctypes
import numpy as np


class STRUCT_STATISTICS(ctypes.Structure):
    _fields_ = [
        ("max", ctypes.c_float),
        ("min", ctypes.c_float),
        ("mean", ctypes.c_float),
        ("median", ctypes.c_float),
        ("std_dev", ctypes.c_float),
        # ("first_der_mean", ctypes.c_float),
        # ("first_der_std_dev", ctypes.c_float),
    ]


STATISTICS_ELEMENT_LENGTH = int(ctypes.sizeof(STRUCT_STATISTICS) / ctypes.sizeof(ctypes.c_float))


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

        assert (library_path is not None and len(library_path) > 0)
        self.libstatistics = ctypes.CDLL(library_path)

    def compute_statistics_across_frames(
            self,
            input_array_float,
            num_frames_uint32,
            bin_length_uint32,
            frame_difference_reciprocal_float,
    ):
        """
        Compute statistics from input_buffer across different frames

        :param input_array_float: must be 1D or 2D np.array of type np.float32
        :param num_frames_uint32: number of frames
        :param bin_length_uint32: bin length
        :param frame_difference_reciprocal_float: non-zero, positive difference between each bin as a reciprocal
        :return: float list of statistics across each frame per bin
        """

        # Check parameters
        assert (self.libstatistics is not None)
        assert (frame_difference_reciprocal_float > 0)
        assert (len(input_array_float.shape) == 1 or len(input_array_float.shape) == 2)
        assert (input_array_float.dtype == np.float32)
        assert (num_frames_uint32 > 0 and bin_length_uint32 > 0)

        # Reshape input_array_float into 1D if necessary
        if len(input_array_float.shape) == 1:
            assert (input_array_float.shape[0] == num_frames_uint32 * bin_length_uint32)
        elif len(input_array_float.shape) == 2:
            assert (input_array_float.shape[0] == num_frames_uint32)
            assert (input_array_float.shape[1] == bin_length_uint32)
            input_array_float = np.reshape(input_array_float, shape=-1, copy=True)
        else:
            assert (False)
        assert (len(input_array_float.shape) == 1)
        assert (input_array_float.shape[0] == num_frames_uint32 * bin_length_uint32)

        # Prepare buffers
        assert (STATISTICS_ELEMENT_LENGTH > 0)
        output_buffer_length = int(STATISTICS_ELEMENT_LENGTH * bin_length_uint32)
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)

        scratch_buffer_length = int(num_frames_uint32 * 2)
        scratch_buffer = (np.zeros(shape=[scratch_buffer_length], dtype=np.float32))

        # Get the return types and argument types
        self.libstatistics.compute_statistics_across_frames.restype = None
        self.libstatistics.compute_statistics_across_frames.argtypes = [
            np.ctypeslib.ndpointer(
                shape=(num_frames_uint32 * bin_length_uint32), dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
            ctypes.c_uint32,
            ctypes.c_float,
            np.ctypeslib.ndpointer(
                shape=[output_buffer_length], dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
            np.ctypeslib.ndpointer(
                shape=[scratch_buffer_length], dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
        ]

        # Run function
        self.libstatistics.compute_statistics_across_frames(
            input_array_float,
            ctypes.c_uint32(num_frames_uint32),
            ctypes.c_uint32(bin_length_uint32),
            ctypes.c_float(frame_difference_reciprocal_float),
            output_buffer,
            ctypes.c_uint32(bin_length_uint32),
            scratch_buffer,
            ctypes.c_uint32(scratch_buffer_length)
        )

        return output_buffer

    def compute_statistics_within_frame(
            self,
            input_array_float,
            bin_length_uint32,
            frame_difference_reciprocal_float,
    ):
        """
        Compute statistics from input_buffer within one frame

        :param input_array_float: must be 1D np.array of type np.float32
        :param bin_length_uint32: bin length; must be bin_length_uint32 <= len(input_list_float)
        :param frame_difference_reciprocal_float: non-zero, positive difference between each bin as a reciprocal
        :return: float list of statistics across all bins
        """

        # Check parameters
        assert (self.libstatistics is not None)
        assert (frame_difference_reciprocal_float > 0)
        assert (input_array_float.shape[0] == bin_length_uint32 and len(input_array_float) == bin_length_uint32)
        assert (input_array_float.dtype == np.float32)

        # Prepare buffers
        assert (STATISTICS_ELEMENT_LENGTH > 0)
        num_frames = 1
        output_buffer_length = int(STATISTICS_ELEMENT_LENGTH * num_frames)
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)

        scratch_buffer_length = int(bin_length_uint32 * 2)
        scratch_buffer = (np.zeros(shape=[scratch_buffer_length], dtype=np.float32))

        # Get the return types and argument types
        self.libstatistics.compute_statistics_within_frame.restype = None
        self.libstatistics.compute_statistics_within_frame.argtypes = [
            np.ctypeslib.ndpointer(
                shape=[bin_length_uint32], dtype=np.float32, ndim=1),
            ctypes.c_uint32,
            ctypes.c_float,
            np.ctypeslib.ndpointer(
                shape=[output_buffer_length], dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
            np.ctypeslib.ndpointer(
                shape=[scratch_buffer_length], dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
        ]

        # Run function
        self.libstatistics.compute_statistics_within_frame(
            input_array_float,
            ctypes.c_uint32(bin_length_uint32),
            ctypes.c_float(frame_difference_reciprocal_float),
            output_buffer,
            ctypes.c_uint32(num_frames),
            scratch_buffer,
            ctypes.c_uint32(scratch_buffer_length)
        )

        return output_buffer
