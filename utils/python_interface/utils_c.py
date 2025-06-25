import ctypes
import numpy as np


class utils_c:
    libutils = None

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
        """

        assert (library_path is not None and len(library_path) > 0)
        self.libutils = ctypes.CDLL(library_path)

    def compute_power_from_complex_arrays(
            self,
            complex_array_float):
        """
        Compute power from an array of complex numbers

        :param complex_array_float:
        :return: float value of power
        """

        # Check parameters
        assert (self.libutils is not None)
        assert (len(complex_array_float) > 0 and len(complex_array_float) % 2 == 0)

        # Grab constants
        input_buffer_length = len(complex_array_float)
        output_buffer_length = np.uint32(int(input_buffer_length / 2))

        # Prepare input buffer with a deep copy
        input_buffer = np.reshape(
            complex_array_float,
            shape=-1,
            copy=True, ).astype(np.float32)
        assert (len(input_buffer) == input_buffer_length)
        assert (len(input_buffer) == input_buffer.shape[0])
        assert (input_buffer.dtype == np.float32)

        # Prepare output buffer
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)
        assert (output_buffer.shape[0] == output_buffer_length)

        # Set the return types and argument types
        self.libutils.compute_power_from_complex_arrays.restype = None
        self.libutils.compute_power_from_complex_arrays.argtypes = [
            np.ctypeslib.ndpointer(
                shape=[input_buffer_length], dtype=np.float32, ndim=1),
            np.ctypeslib.ndpointer(
                shape=[output_buffer_length], dtype=np.float32, ndim=1),
            ctypes.c_uint32,
        ]

        self.libutils.compute_power_from_complex_arrays(
            input_buffer,
            output_buffer,
            ctypes.c_uint32(output_buffer_length),
        )

        return output_buffer

    def compute_magnitude_from_complex_arrays(
            self,
            complex_array_float):
        """
        Compute magnitude from an array of complex numbers

        :param complex_array_float:
        :return: float value of magnitude
        """
        # Check parameters
        assert (self.libutils is not None)
        assert (len(complex_array_float) > 0 and len(complex_array_float) % 2 == 0)

        # Grab constants
        input_buffer_length = len(complex_array_float)
        output_buffer_length = np.uint32(int(input_buffer_length / 2))

        # Prepare input buffer with a deep copy
        input_buffer = np.reshape(
            complex_array_float,
            shape=-1,
            copy=True, ).astype(np.float32)
        assert (len(input_buffer) == input_buffer_length)
        assert (len(input_buffer) == input_buffer.shape[0])
        assert (input_buffer.dtype == np.float32)

        # Prepare output buffer
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)
        assert (output_buffer.shape[0] == output_buffer_length)

        # Set the return types and argument types
        self.libutils.compute_magnitude_from_complex_arrays.restype = None
        self.libutils.compute_magnitude_from_complex_arrays.argtypes = [
            np.ctypeslib.ndpointer(
                shape=[input_buffer_length], dtype=np.float32, ndim=1),
            np.ctypeslib.ndpointer(
                shape=[output_buffer_length], dtype=np.float32, ndim=1),
            ctypes.c_uint32,
        ]

        self.libutils.compute_magnitude_from_complex_arrays(
            input_buffer,
            output_buffer,
            ctypes.c_uint32(output_buffer_length),
        )

        return output_buffer

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
        assert (self.libutils is not None)

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
        assert (self.libutils is not None)

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
        assert (self.libutils is not None)
        assert (target_array_float_list_length_uint32 < len(target_array_float_list))

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
        assert (self.libutils is not None)

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
        assert (self.libutils is not None)

        # Set the return types and argument types
        self.libutils.log2_approximation.restype = ctypes.c_float
        self.libutils.log2_approximation.argtypes = [ctypes.c_float]

        return self.libutils.log2_approximation(
            ctypes.c_float(target_value_float),
        )

    def v_loge_approximation(
            self,
            target_float):
        """
        Performs vector log e approximation. Tries to use vector mathematics if supported

        :param target_float: either float or array of floats
        :return np.array of log e values:
        """
        # Check parameters
        assert (self.libutils is not None)
        assert ((isinstance(target_float, (float, np.float32)) or (
                isinstance(target_float, (list, np.ndarray)) and
                len(target_float) > 0 and isinstance(target_float[0], (float, np.float32)))))

        # Prepare buffer
        if isinstance(target_float, (float, np.float32)):
            loge_buffer = np.ndarray(
                shape=1,
                dtype=np.float32,
            )
            loge_buffer[0] = target_float
        elif isinstance(target_float, (list, np.ndarray)):
            loge_buffer = np.reshape(
                a=target_float,
                shape=-1,
                copy=True,
                order='C',
            )
        else:  # Unsupported type
            loge_buffer = None
        assert (loge_buffer is not None and len(loge_buffer.shape) == 1)
        loge_buffer = loge_buffer[:,]

        # Get the return types and argument types
        self.libutils.v_loge_approximation.restype = None
        self.libutils.v_loge_approximation.argtypes = [
            np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
            ctypes.c_uint32,
        ]

        # Run function
        self.libutils.v_loge_approximation(
            loge_buffer,
            loge_buffer,
            ctypes.c_uint32(len(loge_buffer))
        )

        return loge_buffer

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
        assert (self.libutils is not None)

        # Set the return types and argument types
        self.libutils.square_root_approximation.restype = ctypes.c_float
        self.libutils.square_root_approximation.argtypes = [ctypes.c_float]

        return self.libutils.square_root_approximation(
            ctypes.c_float(target_value_float),
        )
