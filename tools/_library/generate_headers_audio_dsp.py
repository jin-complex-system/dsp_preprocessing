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
        for n_fft in params.n_ffts:
            # Generate Hann Window
            self._generate_audio_dsp_hann_window(
                n_fft=n_fft,
                output_directory=params.output_directory,
                scaling_factor_float=params.scaling_factor_float)

    def _generate_audio_dsp_hann_window(
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

        # Create directory for hann window if necessary
        hann_target_directory = os.path.join(
            output_directory,
            "precomputed_hann"
        )
        os.makedirs(hann_target_directory, exist_ok=True)

        hann_window_cog_dict = eb.get_empty_cog_dict()

        # Export hann window without scaling
        hann_window_name_prefix = "hann_window_no_scale_{}".format(n_fft)
        hann_window_cog_dict["file_prefix"] = hann_window_name_prefix
        hann_window_cog_dict["float_array"] = self.audio_dsp_c.hann_window_compute(
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
            cog_dict=hann_window_cog_dict,
        )

        # Export hann window with scaling
        hann_window_name_prefix = "hann_window_scale_{}".format(n_fft)
        hann_window_cog_dict["file_prefix"] = hann_window_name_prefix
        hann_window_cog_dict["float_array"] = self.audio_dsp_c.hann_window_compute(
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
            cog_dict=hann_window_cog_dict,
        )
