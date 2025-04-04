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

    def hann_window_compute(
            self,
            length_uint32,
            scaling_factor_float=1.0):
        """
        Compute hann window. Recommended to only be called once

        Formula is (0.5 - 0.5 * cos( 2 * PI * iterator / (length - 1))) * scaling_factor

        :param length_uint32: length greater than 1
        :param scaling_factor_float: positive, non-zero scaling factor
        :return: list of hann_window values
        """

        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (length_uint32 > 1)
        assert (scaling_factor_float > 0.0)

        # Prepare buffer
        hann_window_buffer = np.zeros(
            shape=length_uint32, dtype=np.float32)
        assert (len(hann_window_buffer) == length_uint32 and len(hann_window_buffer) == hann_window_buffer.shape[0])
        assert (hann_window_buffer.dtype == np.float32)

        # Set the return types and argument types
        self.libaudiodsp.hann_window_compute.restype = None
        self.libaudiodsp.hann_window_compute.argtypes = [
            np.ctypeslib.ndpointer(
                shape=length_uint32, dtype=np.float32, ndim=1),
            ctypes.c_uint32,
            ctypes.c_float,
        ]

        # Run function
        self.libaudiodsp.hann_window_compute(
            hann_window_buffer,
            ctypes.c_uint32(length_uint32),
            ctypes.c_float(scaling_factor_float),
        )

        return hann_window_buffer

    def compute_power_spectrum(
            self,
            input_samples_array,
            n_fft_uint16,
            window_list=None,
    ):
        """
        Compute power spectrum from input_samples_array

        :param input_samples_array: np.array. Only supports np.int16 at the moment
        :param n_fft_uint16: length of input_samples_array; must be power of 2
        :param window_list: If set, use this window. Otherwise, will use hann_window_compute(n_fft_uint16, 1.0 /np.iinfo(input_samples_array.dtype).max)
        :return: power spectrum array in decibels
        """

        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (n_fft_uint16 % 2 == 0 and n_fft_uint16 >= 2)
        assert (len(input_samples_array) == n_fft_uint16 and input_samples_array.shape[0] == n_fft_uint16)
        assert (window_list is None or len(window_list) == n_fft_uint16)
        assert (input_samples_array.dtype == np.int16)  # Only support np.int16 for now

        # Constants
        input_buffer_length = n_fft_uint16
        output_buffer_length = n_fft_uint16 * 2
        valid_output_buffer_length = int(n_fft_uint16 / 2) + 1
        window_buffer_length = input_buffer_length

        # Prepare hann window buffer if window_list is not provided
        if window_list is None:
            window_buffer = self.hann_window_compute(
                length_uint32=window_buffer_length,
                scaling_factor_float=float(1.0 / np.iinfo(input_samples_array.dtype).max))
        else:
            window_buffer = np.copy(window_list, order='C').astype(np.float32)
        assert (
                len(window_buffer) == window_buffer_length and
                window_buffer.shape[0] == window_buffer_length and
                window_buffer.dtype == np.float32)

        # Make a deep copy to prepare input buffer
        input_buffer = np.copy(input_samples_array, order='C')
        assert (
                input_buffer.dtype == input_samples_array.dtype and
                len(input_buffer) == input_buffer_length and
                input_buffer.shape[0] == input_buffer_length)

        # Prepare output buffer
        output_buffer = np.zeros(shape=output_buffer_length, dtype=np.float32)
        assert (
                len(output_buffer) == output_buffer_length and
                output_buffer.shape[0] == output_buffer_length)

        # Set the return types and argument types
        self.libaudiodsp.compute_power_spectrum_audio_samples_direct.restype = None
        self.libaudiodsp.compute_power_spectrum_audio_samples_direct.argtypes = [
            np.ctypeslib.ndpointer(
                shape=input_buffer_length, dtype=input_samples_array.dtype, ndim=1),
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=output_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint32,
            np.ctypeslib.ndpointer(
                shape=input_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint32,
        ]

        # Run function
        self.libaudiodsp.compute_power_spectrum_audio_samples_direct(
            input_buffer,
            ctypes.c_uint16(input_buffer_length),
            output_buffer,
            ctypes.c_uint32(output_buffer_length),
            window_buffer,
            ctypes.c_uint32(window_buffer_length),
        )

        # Prepare output buffer by chopping off invalid elements
        return np.copy(output_buffer[0:valid_output_buffer_length])

    def convert_frequency_to_mel(
            self,
            frequency,
            htk=False):
        """
        Convert frequency to its equivalent mel, either using HTK or Slaney method

        :param frequency: frequency; will be converted to np.float32
        :param htk: If True, use HTK method instead of Slaney
        :return: mel as np.float32
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (frequency is not None)

        # Prepare input
        frequency_float = np.float32(float(frequency))

        # Use HTK method
        if htk:
            # Set the return types and argument types
            self.libaudiodsp.convert_frequency_to_mel_htk.restype = ctypes.c_float
            self.libaudiodsp.convert_frequency_to_mel_htk.argtypes = [ctypes.c_float]

            # Run function
            mel_float = self.libaudiodsp.convert_frequency_to_mel_htk(
                ctypes.c_float(frequency_float),
            )
        else:
            # Set the return types and argument types
            self.libaudiodsp.convert_frequency_to_mel_slaney.restype = ctypes.c_float
            self.libaudiodsp.convert_frequency_to_mel_slaney.argtypes = [ctypes.c_float]

            # Run function
            mel_float = self.libaudiodsp.convert_frequency_to_mel_slaney(
                ctypes.c_float(frequency_float),
            )

        assert (mel_float is not None and isinstance(mel_float, float))

        return mel_float

    def convert_mel_to_frequency(
            self,
            mel,
            htk=False):
        """
        Convert mel to its equivalent frequency, either using HTK or Slaney method

        :param mel: mel; will be converted to np.float32
        :param htk: If True, use HTK method instead of Slaney
        :return: mel as np.float32
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (mel is not None)

        # Prepare input
        mel_float = np.float32(float(mel))

        # Use HTK method
        if htk:
            # Set the return types and argument types
            self.libaudiodsp.convert_mel_to_frequency_htk.restype = ctypes.c_float
            self.libaudiodsp.convert_mel_to_frequency_htk.argtypes = [ctypes.c_float]

            # Run function
            frequency_float = self.libaudiodsp.convert_mel_to_frequency_htk(
                ctypes.c_float(mel_float),
            )
        else:
            # Set the return types and argument types
            self.libaudiodsp.convert_mel_to_frequency_slaney.restype = ctypes.c_float
            self.libaudiodsp.convert_mel_to_frequency_slaney.argtypes = [ctypes.c_float]

            # Run function
            frequency_float = self.libaudiodsp.convert_mel_to_frequency_slaney(
                ctypes.c_float(mel_float),
            )

        assert (frequency_float is not None and isinstance(frequency_float, float))

        return frequency_float

    def compute_power_spectrum_into_mel_spectrogram(
            self,
            power_spectrum_array,
            n_mel_uint16):
        """
        Compute power spectrum into mel spectrogram
        :param power_spectrum_array:
        :param n_mel_uint16:
        :return: mel_spectrogram
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (
                len(power_spectrum_array) >= n_mel_uint16 and
                power_spectrum_array.shape[0] == len(power_spectrum_array))
        assert (power_spectrum_array.dtype == np.float32)
        assert (isinstance(n_mel_uint16, int))

        # Set constants
        input_buffer_length = len(power_spectrum_array)
        output_buffer_length = n_mel_uint16

        # Make a deep copy to prepare input buffer
        input_buffer = np.copy(power_spectrum_array, order='C')
        assert (
                input_buffer.dtype == power_spectrum_array.dtype and
                len(input_buffer) == input_buffer_length and
                input_buffer.shape[0] == input_buffer_length)


        # Prepare output buffer
        output_buffer = np.zeros(shape=output_buffer_length, dtype=np.float32)
        assert (
                len(output_buffer) == output_buffer_length and
                output_buffer.shape[0] == output_buffer_length)

        # Set the return types and argument types
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram.restype = None
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram.argtypes = [
            np.ctypeslib.ndpointer(
                shape=input_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=output_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
        ]

        # Run function
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram(
            input_buffer,
            ctypes.c_uint16(input_buffer_length),
            output_buffer,
            ctypes.c_uint16(output_buffer_length)
        )
        return output_buffer
