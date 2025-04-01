import ctypes

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

        Formula is log2(real_value ^ 2 + img_value ^ 2) / (2 * log2(10)) + log_scale_factor

        :param real_value_float:
        :param img_value_float:
        :param scale_factor_float: scale factor applied to the magnitude
        :return: magnitude
        """

        assert(self.libutils is not None)
        return self.libutils.compute_magnitude_from_complex(
            ctypes.c_float(real_value_float),
            ctypes.c_float(img_value_float),
            ctypes.c_float(scale_factor_float),
        )

    def compute_log_from_complex(
            self,
            real_value_float,
            img_value_float,
            log_scale_factor):
        """
        Compute logarithmic from complex number

        Assumes that precision loss is acceptable, and
        comparison operations are overall cheaper to run.

        Formula is log2(real_value ^ 2 + img_value ^ 2) / (2 * log2(10)) + log_scale_factor

        :param real_value_float:
        :param img_value_float:
        :param log_scale_factor: scale factor applied to the magnitude
        :return: logarithimic magnitude
        """

        assert(self.libutils is not None)
        return self.libutils.compute_log_from_complex(
            ctypes.c_float(real_value_float),
            ctypes.c_float(img_value_float),
            ctypes.c_float(log_scale_factor),
        )

    def log_scale_factor(
            self,
            real_value_float,
            img_value_float,
            log_scale_factor):
        """
        Compute decibels from complex number

        Assumes that precision loss is acceptable, and
        comparison operations are overall cheaper to run.

        Formula is (10 / log2(10)) * log2(real_value ^ 2 + img_value ^ 2) + log_scale_factor

        :param real_value_float:
        :param img_value_float:
        :param log_scale_factor: scale factor applied to the magnitude
        :return: decibel magnitude
        """

        assert(self.libutils is not None)
        return self.libutils.compute_decibels_from_complex(
            ctypes.c_float(real_value_float),
            ctypes.c_float(img_value_float),
            ctypes.c_float(log_scale_factor),
        )

    def insertion_sort(
            self,
            target_value_float,
            sorted_array_float_list):
        """
        Conduct insertion sort into a sorted array

        :param target_value_float:
        :param sorted_array_float_list:
        :return:
        """

        assert(self.libutils is not None)

        sorted_array_float_pointer = ctypes.c_float_p(sorted_array_float_list)
        array_length = len(sorted_array_float_list)
        assert(array_length > 0)

        return self.libutils.insertion_sort(
            ctypes.c_float(target_value_float),
            sorted_array_float_pointer,
            ctypes.uint(array_length),
        )

    def log10_approximation(
            self,
            target_value_float):
        """
        Performs log10 approximation.

        :param target_value_float: Positive floats. If 0.0f, returns -37.929779052734375f immediately
        :return:
        """

        assert(self.libutils is not None)
        return self.libutils.log10_approximation(
            ctypes.c_float(target_value_float),
        )

    def log2_approximation(
            self,
            target_value_float):
        """
        Performs log2 approximation.

        :param target_value_float: Positive floats. If 0.0f, returns -126.0f immediately
        :return:
        """

        assert(self.libutils is not None)
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

        assert(self.libutils is not None)
        return self.libutils.square_root_approximation(
            ctypes.c_float(target_value_float),
        )

if __name__ == "__main__":
    UTILS_DLL_LIBRARY = "../../cmake-build-debug/utils/libutils.dll"

    my_types = utils_c(library_path=UTILS_DLL_LIBRARY)

    print(my_types.compute_magnitude_from_complex(1.0, 0.0, 1.0))

