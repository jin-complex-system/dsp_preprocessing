import importlib.util
import sys
import _library.export_buffer as eb
import os


class GenerateAudioDSP:
    def __init__(
            self,
            audio_dsp_python_interface_filepath,
            audio_dsp_library_binary_filepath,
    ):
        # Import audio_dsp_c module
        audio_dsp_spec = importlib.util.spec_from_file_location(
            "audio_dsp",
            audio_dsp_python_interface_filepath)
        audio_dsp = importlib.util.module_from_spec(audio_dsp_spec)
        sys.modules["audio_dsp"] = audio_dsp
        audio_dsp_spec.loader.exec_module(audio_dsp)
        self.audio_dsp_c = audio_dsp.audio_dsp_c(audio_dsp_library_binary_filepath)

    def generate_headers(
            self,
            params):
        """
        Given the parameters, precompute values and export them
        :param params: defined in parameters.py
        :return:
        """

        mel_precompute_dict = eb.get_empty_mel_precompute_cog_dict()
        assert (mel_precompute_dict["data_structures"] == list())

        for n_fft in params.n_ffts:
            self._generate_audio_dsp_window(
                n_fft=n_fft,
                output_directory=params.output_directory,
                scaling_factor_float=params.scaling_factor_float)

            for sample_rate in params.sample_rates:
                for n_mel in params.n_mels:
                    self._generate_audio_dsp_mel_constants(
                        n_mel=n_mel,
                        n_fft=n_fft,
                        sample_rate=sample_rate,
                        output_directory=params.output_directory,
                    )

                    mel_precompute_struct = eb.MelPrecomputeStructure()
                    mel_precompute_struct.n_mel = n_mel
                    mel_precompute_struct.n_fft = n_fft
                    mel_precompute_struct.sample_rate = sample_rate

                    mel_precompute_dict["data_structures"].append(mel_precompute_struct)

        # Export .c file
        eb.export_mel_precompute_c_file(
            target_filepath=os.path.join(params.output_directory, "mel_get_precomputed.c"),
            mel_precompute_cog_dict=mel_precompute_dict,
        )

    def _generate_audio_dsp_mel_constants(
            self,
            n_mel,
            n_fft,
            sample_rate,
            output_directory
    ):
        """
        Generate mel constants for given values

        :param n_mel:
        :param n_fft:
        :param sample_rate:
        :param output_directory:

        :return:
        """
        # Create directory for mel constants
        mel_target_directory = os.path.join(
            output_directory,
            "precomputed_mel"
        )
        os.makedirs(mel_target_directory, exist_ok=True)

        mel_constants_cog_dict = eb.get_empty_cog_dict()

        # Generate mel constants
        (mel_centre_float,
         mel_centre_next,
         mel_centre_prev,
         mel_weights) = self.audio_dsp_c.compute_mel_spectrogram_bins(
            n_mel_uint16=n_mel,
            n_fft_uint16=n_fft,
            sample_rate_uint16=sample_rate,
        )
        assert (len(mel_centre_float) == n_mel + 1)
        assert (len(mel_centre_next) == n_mel - 1)
        assert (len(mel_centre_prev) == n_mel - 1)
        assert (len(mel_weights) == n_mel - 1)

        # Export centre float
        mel_centre_prefix = "mel_centre_frequencies_float_mel_{}_fft_{}_sr_{}".format(n_mel, n_fft, sample_rate)
        mel_constants_cog_dict["file_prefix"] = mel_centre_prefix
        mel_constants_cog_dict["data_array"] = mel_centre_float
        mel_constants_cog_dict["comment_strings"] = ""
        eb.export_ndarray(
            target_filepath=os.path.join(
                mel_target_directory,
                "mel_centre_frequencies_float",
                "{}.h".format(mel_centre_prefix)
            ),
            ndarray_type="float",
            cog_dict=mel_constants_cog_dict,
        )

        # Export centre next
        mel_next_bin_prefix = "mel_centre_frequencies_next_bin_mel_{}_fft_{}_sr_{}".format(n_mel, n_fft, sample_rate)
        mel_constants_cog_dict["file_prefix"] = mel_next_bin_prefix
        mel_constants_cog_dict["data_array"] = mel_centre_next
        mel_constants_cog_dict["comment_strings"] = ""
        eb.export_ndarray(
            target_filepath=os.path.join(
                mel_target_directory,
                "mel_centre_frequencies_next_bin",
                "{}.h".format(mel_next_bin_prefix)
            ),
            ndarray_type="uint16",
            cog_dict=mel_constants_cog_dict,
        )

        # Export centre prev
        mel_prev_bin_prefix = "mel_centre_frequencies_prev_bin_mel_{}_fft_{}_sr_{}".format(n_mel, n_fft, sample_rate)
        mel_constants_cog_dict["file_prefix"] = mel_prev_bin_prefix
        mel_constants_cog_dict["data_array"] = mel_centre_prev
        mel_constants_cog_dict["comment_strings"] = ""
        eb.export_ndarray(
            target_filepath=os.path.join(
                mel_target_directory,
                "mel_centre_frequencies_prev_bin",
                "{}.h".format(mel_prev_bin_prefix)
            ),
            ndarray_type="uint16",
            cog_dict=mel_constants_cog_dict,
        )

        # Export weights
        mel_weights_prefix = "mel_frequency_weights_mel_{}_fft_{}_sr_{}".format(n_mel, n_fft, sample_rate)
        mel_constants_cog_dict["file_prefix"] = mel_weights_prefix
        mel_constants_cog_dict["data_array"] = mel_weights
        mel_constants_cog_dict["comment_strings"] = ""
        eb.export_ndarray(
            target_filepath=os.path.join(
                mel_target_directory,
                "mel_weights",
                "{}.h".format(mel_weights_prefix)
            ),
            ndarray_type="float",
            cog_dict=mel_constants_cog_dict,
        )

    def _generate_audio_dsp_window(
            self,
            n_fft,
            output_directory,
            scaling_factor_float):
        """

        :param n_fft:
        :param output_directory:
        :param scaling_factor_float:
        :return:
        """

        # Create directory for precomputed window
        hann_target_directory = os.path.join(
            output_directory,
            "precomputed_window"
        )
        os.makedirs(hann_target_directory, exist_ok=True)

        hann_window_cog_dict = eb.get_empty_cog_dict()

        # Export hann window without scaling
        hann_window_name_prefix = "hann_window_no_scale_{}".format(n_fft)
        hann_window_cog_dict["file_prefix"] = hann_window_name_prefix
        hann_window_cog_dict["data_array"] = self.audio_dsp_c.hann_window_compute(
            length_uint32=n_fft,
            scaling_factor_float=1.0,
        )
        hann_window_cog_dict["comment_strings"] = ""
        eb.export_ndarray(
            target_filepath=os.path.join(
                hann_target_directory,
                "hann_window",
                "{}.h".format(hann_window_name_prefix)
            ),
            ndarray_type="float",
            cog_dict=hann_window_cog_dict,
        )

        # Export hann window with scaling
        hann_window_name_prefix = "hann_window_scale_{}".format(n_fft)
        hann_window_cog_dict["file_prefix"] = hann_window_name_prefix
        hann_window_cog_dict["data_array"] = self.audio_dsp_c.hann_window_compute(
            length_uint32=n_fft,
            scaling_factor_float=scaling_factor_float,
        )
        hann_window_cog_dict["comment_strings"] = ""
        eb.export_ndarray(
            target_filepath=os.path.join(
                hann_target_directory,
                "hann_window",
                "{}.h".format(hann_window_name_prefix)
            ),
            ndarray_type="float",
            cog_dict=hann_window_cog_dict,
        )
