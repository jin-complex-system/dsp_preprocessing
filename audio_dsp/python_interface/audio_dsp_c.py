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
            copy=True, ).astype(np.float32)
        assert (len(input_buffer) == len(real_input_array))
        assert (len(input_buffer) == input_buffer.shape[0])
        assert (input_buffer.dtype == np.float32)

        # Prepare buffer lengths
        n_fft = len(input_buffer)
        output_buffer_length = 2 * len(input_buffer)

        # Prepare output buffer
        output_buffer = np.zeros(shape=[output_buffer_length], dtype=np.float32)
        assert (output_buffer.shape[0] == output_buffer_length)

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
        num_actual_valid_results = int(n_fft / 2 + 1) * 2
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
        :param window_list: If set, use this window. Otherwise,
        use hann_window_compute(n_fft_uint16, 1.0 /np.iinfo(input_samples_array.dtype).max)
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

    def compute_mel_spectrogram_bins(
            self,
            n_mel_uint16,
            n_fft_uint16,
            sample_rate_uint16,
            max_frequency_uint16):
        """
        Compute the mel spectrogram bins
        :param n_mel_uint16: number of mel bins in uint16_t
        :param n_fft_uint16: number of FFT bins in uint16_t; must be power of 2
        :param sample_rate_uint16: sample rate of the audio signal
        :param max_frequency_uint16: the range of mel bins up to sample_rate_uint16 / 2; if set to 0, use sample_rate_uint16 / 2

        :return:
        mel_centre_freq_float_buffer - for the current index the actual centre of bin as a float
        mel_centre_freq_next_bin_buffer - for the current index the next bin centre position index
        mel_centre_freq_prev_bin_buffer - for the current index the prev bin centre position index
        mel_freq_weights_buffer - for the current index the weights assigned to the mel bin
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (isinstance(n_mel_uint16, int) and n_mel_uint16 > 0)
        assert (isinstance(n_fft_uint16, int) and n_fft_uint16 % 2 == 0 and n_fft_uint16 > 1)
        assert (isinstance(sample_rate_uint16, int) and sample_rate_uint16 > 0)
        assert (isinstance(max_frequency_uint16, int) and
                (max_frequency_uint16 == 0 or max_frequency_uint16 <= sample_rate_uint16 / 2))

        # Prepare constants
        mel_centre_freq_float_buffer_length = n_mel_uint16 + 1
        mel_centre_freq_next_bin_buffer_length = n_mel_uint16 - 1
        mel_centre_freq_prev_bin_buffer_length = n_mel_uint16 - 1
        mel_freq_weights_buffer_length = n_mel_uint16 - 1

        # Prepare buffers
        mel_centre_freq_float_buffer = np.zeros(shape=mel_centre_freq_float_buffer_length, dtype=np.float32)
        assert (
                len(mel_centre_freq_float_buffer) == mel_centre_freq_float_buffer_length and
                mel_centre_freq_float_buffer.shape[0] == mel_centre_freq_float_buffer_length)
        mel_centre_freq_next_bin_buffer = np.zeros(shape=mel_centre_freq_next_bin_buffer_length, dtype=np.uint16)
        assert (
                len(mel_centre_freq_next_bin_buffer) == mel_centre_freq_next_bin_buffer_length and
                mel_centre_freq_next_bin_buffer.shape[0] == mel_centre_freq_next_bin_buffer_length)
        mel_centre_freq_prev_bin_buffer = np.zeros(shape=mel_centre_freq_prev_bin_buffer_length, dtype=np.uint16)
        assert (
                len(mel_centre_freq_prev_bin_buffer) == mel_centre_freq_prev_bin_buffer_length and
                mel_centre_freq_prev_bin_buffer.shape[0] == mel_centre_freq_prev_bin_buffer_length)
        mel_freq_weights_buffer = np.zeros(shape=mel_freq_weights_buffer_length, dtype=np.float32)
        assert (
                len(mel_freq_weights_buffer) == mel_freq_weights_buffer_length and
                mel_freq_weights_buffer.shape[0] == mel_freq_weights_buffer_length)

        # Set the return types and argument types
        self.libaudiodsp.compute_mel_spectrogram_bins.restype = None
        self.libaudiodsp.compute_mel_spectrogram_bins.argtypes = [
            ctypes.c_uint16,
            ctypes.c_uint16,
            ctypes.c_uint16,
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=mel_centre_freq_float_buffer_length, dtype=np.float32, ndim=1),
            np.ctypeslib.ndpointer(
                shape=mel_centre_freq_next_bin_buffer_length, dtype=np.uint16, ndim=1),
            np.ctypeslib.ndpointer(
                shape=mel_centre_freq_prev_bin_buffer_length, dtype=np.uint16, ndim=1),
            np.ctypeslib.ndpointer(
                shape=mel_freq_weights_buffer_length, dtype=np.float32, ndim=1),
        ]

        # Run function
        self.libaudiodsp.compute_mel_spectrogram_bins(
            ctypes.c_uint16(n_mel_uint16),
            ctypes.c_uint16(n_fft_uint16),
            ctypes.c_uint16(sample_rate_uint16),
            ctypes.c_uint16(max_frequency_uint16),
            mel_centre_freq_float_buffer,
            mel_centre_freq_next_bin_buffer,
            mel_centre_freq_prev_bin_buffer,
            mel_freq_weights_buffer,
        )

        return (
            mel_centre_freq_float_buffer,
            mel_centre_freq_next_bin_buffer,
            mel_centre_freq_prev_bin_buffer,
            mel_freq_weights_buffer)

    def compute_power_spectrum_into_mel_spectrogram_raw(
            self,
            power_spectrum_array,
            n_mel_uint16,
            n_fft_uint16,
            sample_rate_uint16,
            max_frequency_uint16):
        """
        Compute power spectrum into mel spectrogram; do not use precomputed values
        :param power_spectrum_array: power spectrum computed with compute_power_spectrum()
        :param n_mel_uint16: number of mel bins in uint16_t
        :param n_fft_uint16: number of FFT bins in uint16_t; must be power of 2
        :param sample_rate_uint16: sample rate of the audio signal
        :param max_frequency_uint16: the range of mel bins up to sample_rate_uint16 / 2; if set to 0, use samp
        :return: mel_spectrogram, max_mel_value
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (
                n_mel_uint16 <= len(power_spectrum_array) == power_spectrum_array.shape[0])
        assert (power_spectrum_array.dtype == np.float32)
        assert (isinstance(n_mel_uint16, int) and n_mel_uint16 > 0)
        assert (isinstance(n_fft_uint16, int) and n_fft_uint16 > 0)
        assert (isinstance(sample_rate_uint16, int) and sample_rate_uint16 > 0)
        assert (isinstance(max_frequency_uint16, int) and
                (max_frequency_uint16 == 0 or max_frequency_uint16 <= sample_rate_uint16 / 2))

        # Set constants
        input_buffer_length = len(power_spectrum_array)
        output_buffer_length = n_mel_uint16
        scratch_buffer_length = n_mel_uint16 * 4 + 2

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

        # Prepare scratch buffer
        scratch_buffer = np.zeros(shape=scratch_buffer_length, dtype=np.float32)
        assert (
                len(scratch_buffer) == scratch_buffer_length and
                scratch_buffer.shape[0] == scratch_buffer_length)

        # Set the return types and argument types
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram_raw.restype = ctypes.c_float
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram_raw.argtypes = [
            np.ctypeslib.ndpointer(
                shape=input_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            ctypes.c_uint16,
            ctypes.c_uint16,
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=output_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=scratch_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
        ]

        # Run function
        max_mel_value = self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram_raw(
            input_buffer,
            ctypes.c_uint16(input_buffer_length),
            ctypes.c_uint16(n_fft_uint16),
            ctypes.c_uint16(sample_rate_uint16),
            ctypes.c_uint16(max_frequency_uint16),
            output_buffer,
            ctypes.c_uint16(output_buffer_length),
            scratch_buffer,
            ctypes.c_uint16(scratch_buffer_length),
        )

        return (
            output_buffer,
            float(max_mel_value))

    def compute_power_spectrum_into_mel_spectrogram(
            self,
            power_spectrum_array,
            n_mel_uint16,
            n_fft_uint16,
            sample_rate_uint16,
            max_frequency_uint16):
        """
        Compute power spectrum into mel spectrogram with precomputed values (if available)
        :param power_spectrum_array: power spectrum computed with compute_power_spectrum()
        :param n_mel_uint16:
        :param n_fft_uint16:
        :param sample_rate_uint16: sample rate of the audio signal
        :param max_frequency_uint16: the range of mel bins up to sample_rate_uint16 / 2; if set to 0, use samp
        :return: mel_spectrogram, max_mel_value
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (
                n_mel_uint16 <= len(power_spectrum_array) == power_spectrum_array.shape[0])
        assert (power_spectrum_array.dtype == np.float32)
        assert (isinstance(n_mel_uint16, int) and n_mel_uint16 > 0)
        assert (isinstance(n_fft_uint16, int) and n_fft_uint16 > 0)
        assert (isinstance(max_frequency_uint16, int) and
                (max_frequency_uint16 == 0 or max_frequency_uint16 <= sample_rate_uint16 / 2))

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
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram.restype = ctypes.c_float
        self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram.argtypes = [
            np.ctypeslib.ndpointer(
                shape=input_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            np.ctypeslib.ndpointer(
                shape=output_buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            ctypes.c_uint16,
            ctypes.c_uint16,
            ctypes.c_uint16,
        ]

        # Run function
        max_mel_value = self.libaudiodsp.compute_power_spectrum_into_mel_spectrogram(
            input_buffer,
            ctypes.c_uint16(input_buffer_length),
            output_buffer,
            ctypes.c_uint16(n_fft_uint16),
            ctypes.c_uint16(sample_rate_uint16),
            ctypes.c_uint16(max_frequency_uint16),
            ctypes.c_uint16(n_mel_uint16),
        )

        return (
            output_buffer,
            float(max_mel_value))

    def convert_power_to_decibel(
            self,
            spectrogram_array_float32,
            spectrogram_array_length_uint16,
            reference_power_float32,
            top_decibel_float32=None):
        """
        Compute in-place spectrogram to decibel units. Handles negative values

        Formula is roughly spectrogram_array[] = 10 * log10(spectrogram_array[]) - 10 * log10(reference_power)

        :param spectrogram_array_float32:
        :param spectrogram_array_length_uint16:
        :param reference_power_float32: non-zero, positive value that is defined as 0 dB
        :param top_decibel_float32: if non-zero and positive, clip values to top_decibel.
        :return: decibels-scaled spectrogram np array
        """
        # Check parameters
        assert (self.libaudiodsp is not None)
        assert (isinstance(spectrogram_array_length_uint16, int))
        assert (
                spectrogram_array_length_uint16 <= len(spectrogram_array_float32) == spectrogram_array_float32.shape[0])
        assert (spectrogram_array_float32.dtype == np.float32)
        assert (
                (isinstance(top_decibel_float32, float) and (top_decibel_float32 == -1.0 or top_decibel_float32 > 0.0))
                or top_decibel_float32 is None  # Matches parity with librosa.power_to_db()
        )

        # Set constants
        buffer_length = int(spectrogram_array_length_uint16)
        reference_power = float(reference_power_float32)

        top_decibel = np.float32(-1.0)
        if top_decibel_float32 is not None:
            top_decibel = np.float32(top_decibel_float32)
        assert (top_decibel.dtype == np.float32)

        # Make a deep copy to prepare buffer
        buffer = np.copy(a=spectrogram_array_float32, order='C')
        assert (
                buffer.dtype == spectrogram_array_float32.dtype and
                len(buffer) == buffer_length and
                buffer.shape[0] == buffer_length)

        # Set the return types and argument types
        self.libaudiodsp.convert_power_to_decibel.restype = None
        self.libaudiodsp.convert_power_to_decibel.argtypes = [
            np.ctypeslib.ndpointer(
                shape=buffer_length, dtype=np.float32, ndim=1),
            ctypes.c_uint16,
            ctypes.c_float,
            ctypes.c_float,
        ]

        # Run function
        self.libaudiodsp.convert_power_to_decibel(
            buffer,
            ctypes.c_uint16(buffer_length),
            ctypes.c_float(reference_power),
            ctypes.c_float(top_decibel),
        )

        return buffer
