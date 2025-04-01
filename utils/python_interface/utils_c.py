import ctypes
import numpy as np

class utils_c:
    libutils = None
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
        """

        assert(library_path is not None and len(library_path) > 0)
        self.libutils = ctypes.CDLL(library_path)

    def compute_magnitude_from_complex(
            self,
            real_value_float,
            img_value_float,
            scale_factor_float):
        """
        Compute magnitude from a complex number

        Assumes that precision loss is acceptable, and
        comparison and fabs() operations are overall cheaper to run

        :param real_value_float:
        :param img_value_float:
        :param scale_factor_float: scale factor applied to the magnitude
        :return: float value of magnitude
        """
        # Check parameters
        assert(self.libutils is not None)

        # Set the return types and argument types
        self.libutils.compute_magnitude_from_complex.restype = ctypes.c_float
        self.libutils.compute_magnitude_from_complex.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float]

        return self.libutils.compute_magnitude_from_complex(
            ctypes.c_float(real_value_float),
            ctypes.c_float(img_value_float),
            ctypes.c_float(scale_factor_float),
        )

    def compute_log_from_complex(
            self,
            real_value_float,
            img_value_float,
            log_scale_factor_float):
        """
        Compute logarithmic from complex number

        Assumes that precision loss is acceptable, and
        comparison operations are overall cheaper to run.

        Formula is log2(real_value ^ 2 + img_value ^ 2) / (2 * log2(10)) + log_scale_factor

        :param real_value_float:
        :param img_value_float:
        :param log_scale_factor_float: scale factor applied to the magnitude
        :return: float value of logarithmic magnitude
        """
        # Check parameters
        assert(self.libutils is not None)

        # Set the return types and argument types
        self.libutils.compute_log_from_complex.restype = ctypes.c_float
        self.libutils.compute_log_from_complex.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float]

        return self.libutils.compute_log_from_complex(
            ctypes.c_float(real_value_float),
            ctypes.c_float(img_value_float),
            ctypes.c_float(log_scale_factor_float),
        )

    def compute_decibels_from_complex(
            self,
            real_value_float,
            img_value_float,
            log_scale_factor_float):
        """
        Compute decibels from complex number

        Assumes that precision loss is acceptable, and
        comparison operations are overall cheaper to run.

        Formula is (10 / log2(10)) * log2(real_value ^ 2 + img_value ^ 2) + log_scale_factor

        :param real_value_float:
        :param img_value_float:
        :param log_scale_factor_float: scale factor applied to the magnitude
        :return: float value of decibel magnitude
        """
        # Check parameters
        assert(self.libutils is not None)

        # Set the return types and argument types
        self.libutils.compute_decibels_from_complex.restype = ctypes.c_float
        self.libutils.compute_decibels_from_complex.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float]

        return self.libutils.compute_decibels_from_complex(
            ctypes.c_float(real_value_float),
            ctypes.c_float(img_value_float),
            ctypes.c_float(log_scale_factor_float),
        )

    def insertion_sort(
            self,
            target_value_float,
            target_array_float_list,
            target_array_float_list_length_uint32):
        """
        Conduct insertion sort into a sorted array

        :param target_value_float: float value to be inserted
        :param target_array_float_list: target list to be inserted; will be changed after function is called
        :param target_array_float_list_length_uint32: length of valid values inside target_array_float_list; must be less than len(target_array_float_list)
        :return: None
        """

        # Check parameters
        assert(self.libutils is not None)
        assert(target_array_float_list_length_uint32 < len(target_array_float_list))

        # Set the return types and argument types
        self.libutils.insertion_sort.restype = None
        self.libutils.insertion_sort.argtypes = [
            ctypes.c_float,
            np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32]

        self.libutils.insertion_sort(
            ctypes.c_float(target_value_float),
            target_array_float_list,
            ctypes.c_uint32(target_array_float_list_length_uint32),
        )

    def log10_approximation(
            self,
            target_value_float):
        """
        Performs log10 approximation.

        :param target_value_float: Positive floats. If 0.0f, returns -37.929779052734375f immediately
        :return: float value of log10
        """
        # Check parameters
        assert(self.libutils is not None)

        # Set the return types and argument types
        self.libutils.log10_approximation.restype = ctypes.c_float
        self.libutils.log10_approximation.argtypes = [ctypes.c_float]

        return self.libutils.log10_approximation(
            ctypes.c_float(target_value_float),
        )

    def log2_approximation(
            self,
            target_value_float):
        """
        Performs log2 approximation.

        :param target_value_float: Positive floats. If 0.0f, returns -126.0f immediately
        :return: float value of log2
        """
        # Check parameters
        assert(self.libutils is not None)

        # Get the return types and argument types
        self.libutils.log2_approximation.restype = ctypes.c_float
        self.libutils.log2_approximation.argtypes = [ctypes.c_float]

        return self.libutils.log2_approximation(
            ctypes.c_float(target_value_float),
        )

    def square_root_approximation(
            self,
            target_value_float):
        """
        Perform square root approximation

        Precision loss is acceptable

        :param target_value_float:
        :return: sqrt(float)
        """
        # Check parameters
        assert(self.libutils is not None)

        # Set the return types and argument types
        self.libutils.square_root_approximation.restype = ctypes.c_float
        self.libutils.square_root_approximation.argtypes = [ctypes.c_float]

        return self.libutils.square_root_approximation(
            ctypes.c_float(target_value_float),
        )
