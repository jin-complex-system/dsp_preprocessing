import ctypes

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
        Compute statistics from input_buffer within one frame

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
            bin_length_uint32,
            frame_difference_reciprocal_float,
    ):
        """
        Compute statistics from input_buffer across different frames

        :param input_list_float:
        :param bin_length_uint32:
        :param frame_difference_reciprocal_float: non-zero, positive difference between each bin as a reciprocal
        :return:
        """

        # TODO: Implement this function

        return

