import librosa
import importlib.util
import _library.export_buffer as eb
import sys
import os
import numpy as np
import math
from generate_audio_file_as_header import generate_audio_file_as_header

BINARY_LOCATION = sys.argv[1]
AUDIO_DSP_PYTHON_INTERFACE = sys.argv[2]
# AUDIO_FILEPATH = sys.argv[3]


def save_plots(
        target_array,
        target_filepath,
):
    """
    Save plots from audio processing arrays

    :param target_array:
    :param target_filepath:
    :return:
    """
    # Import necessary libraries
    import matplotlib.pyplot as plt
    import librosa
    import os

    # Create output directory if it does not exist before
    os.makedirs(os.path.dirname(target_filepath), exist_ok=True)

    plt.figure()
    librosa.display.specshow(
        target_array,
        cmap="magma")
    plt.axis("off")
    plt.savefig(
        target_filepath,
        bbox_inches='tight',
        pad_inches=0)
    plt.close()


def _import_audio_dsp_c_module(
        audio_dsp_python_interface_filepath,
        audio_dsp_library_binary_filepath):
    """
    Import audio_dsp_c module
    :param audio_dsp_python_interface_filepath:
    :param audio_dsp_library_binary_filepath:
    :return:
    """

    # Import audio_dsp_c module
    audio_dsp_spec = importlib.util.spec_from_file_location(
        "audio_dsp",
        audio_dsp_python_interface_filepath)
    audio_dsp = importlib.util.module_from_spec(audio_dsp_spec)
    sys.modules["audio_dsp"] = audio_dsp
    audio_dsp_spec.loader.exec_module(audio_dsp)
    audio_dsp_c = audio_dsp.audio_dsp_c(audio_dsp_library_binary_filepath)
    return audio_dsp_c


def load_audio_and_resample(
        audio_filepath,
        sample_rate,
):
    """
    Load audio using librosa and resample with chosen sample_rate
    :param audio_filepath:
    :param sample_rate:
    :return:
    """

    # Check parameters
    assert (len(audio_filepath) > 0)
    assert (sample_rate > 0)

    audio_samples, original_sample_rate = librosa.load(
        path=audio_filepath,
        sr=None,
        mono=True,
        dtype=np.float32)
    audio_samples = librosa.resample(
        y=audio_samples,
        orig_sr=original_sample_rate,
        target_sr=sample_rate,
        res_type="soxr_vhq",
        scale=True,
    )

    return audio_samples


def generate_mel_spectrogram_and_buffers(
        audio_dsp_c_lib,
        audio_int16,
        sample_rate,
        n_fft,
        hop_length,
        max_num_frames,
        n_mels,
        fmax):
    """

    :param audio_dsp_c_lib:
    :param audio_int16:
    :param sample_rate:
    :param n_fft:
    :param hop_length:
    :param max_num_frames:
    :param n_mels:
    :param fmax:
    :return:
    """

    # Preprocessing Constants
    num_samples = len(audio_int16)
    actual_num_frames = math.floor(num_samples / hop_length - n_fft / hop_length) + 1

    left_padding_length = 0
    # Will need to pad
    if actual_num_frames < max_num_frames:
        num_frames = actual_num_frames

        left_padding_length = int((max_num_frames - actual_num_frames) / 2)

        print("Padding length: {}".format(left_padding_length))
    # Will need to crop so process less frames
    else:
        num_frames = max_num_frames
        print("Cropping {} to {}".format(actual_num_frames, num_frames))
    assert (num_frames > 0)

    scaling_factor_float = float(1.0 / np.iinfo(np.int16).max)
    hann_window_scaled = audio_dsp_c_lib.hann_window_compute(
        length_uint32=n_fft,
        scaling_factor_float=scaling_factor_float,
    )

    # Generate buffers
    power_spectrum = np.zeros((math.floor(n_fft / 2) + 1, max_num_frames), dtype=np.float32)
    mel_spec = np.zeros(shape=(n_mels, max_num_frames), dtype=np.float32)

    # Compute power spectrum
    for frame_iterator in range(num_frames):
        begin_iterator = frame_iterator * hop_length
        end_iterator = begin_iterator + n_fft

        # Note that we do not pad sample_frame with zeros
        power_spectrum[:, frame_iterator] = audio_dsp_c_lib.compute_power_spectrum(
            input_samples_array=audio_int16[begin_iterator:end_iterator],
            n_fft_uint16=n_fft,
            window_list=hann_window_scaled,
        )

    # Compute mel spectrogram
    top_mel = 1e-16
    for frame_iterator in range(num_frames):
        mel_frame_iterator = frame_iterator + left_padding_length

        mel_spec[:, mel_frame_iterator], max_mel = (
            audio_dsp_c_lib.compute_power_spectrum_into_mel_spectrogram(
                power_spectrum_array=power_spectrum[:, frame_iterator],
                n_mel_uint16=n_mels,
                n_fft_uint16=n_fft,
                sample_rate_uint16=sample_rate,
                max_frequency_uint16=fmax,
            ))
        if max_mel > top_mel:
            top_mel = max_mel
    mel_spec = np.reshape(mel_spec, newshape=-1)  # In older python versions, use newshape

    assert(top_mel > 1e-16)

    # Convert power to decibel, and normalise to [0, 255] as uint8
    scaled_mel_spec_uint = audio_dsp_c_lib.convert_power_to_decibel_scale(
        spectrogram_array_float32=mel_spec,
        spectrogram_array_length_uint32=len(mel_spec),
        reference_power_float32=top_mel,
    )
    assert (scaled_mel_spec_uint.dtype == np.uint8)
    assert (scaled_mel_spec_uint.shape == mel_spec.shape)
    assert (len(scaled_mel_spec_uint) == len(mel_spec))

    return power_spectrum, mel_spec, scaled_mel_spec_uint


def generate_edge_unit_tests():
    # Load library
    audio_dsp_c_lib = _import_audio_dsp_c_module(
        audio_dsp_python_interface_filepath=AUDIO_DSP_PYTHON_INTERFACE,
        audio_dsp_library_binary_filepath=BINARY_LOCATION,
    )

    audio_basename = "us8k_24074_1_0_2"
    audio_filepath = os.path.join("tools", "audio", "{}.wav".format(audio_basename))

    # Fixed preprocessing constants
    sample_rate = 44100
    audio_integer_type = np.int16
    n_fft = 2048
    hop_length = 512
    num_seconds_audio = 7
    maximum_length_audio = sample_rate * num_seconds_audio
    desired_num_frames = int(maximum_length_audio / hop_length - n_fft / hop_length + 1)
    n_mels = 64
    fmax = 8000
    cropped_num_frames = 384

    if cropped_num_frames < desired_num_frames:
        max_num_frames = cropped_num_frames
    else:
        max_num_frames = desired_num_frames

    # Compute intermediate constants
    fft_output_length_per_frame = int(n_fft / 2) + 1
    power_spectrum_expected_shape = (fft_output_length_per_frame, cropped_num_frames)
    mel_spectrogram_expected_shape = (n_mels, cropped_num_frames)

    # Misc constants
    absolute_error_tolerance_for_integer = 2
    output_directory = os.path.join("_output_edge_unit_tests", audio_basename)

    # Load audio file
    original_audio_samples = load_audio_and_resample(
        audio_filepath=audio_filepath,
        sample_rate=sample_rate)
    print("Audio filepath: {}, duration: {}s, sample_rate: {}, num_samples: {}".format(
        audio_filepath,
        len(original_audio_samples) / sample_rate,
        sample_rate,
        len(original_audio_samples)
    ))

    # Export audio file as a header
    generate_audio_file_as_header(
        audio_filepath=audio_filepath,
        audio_type=audio_integer_type,
        output_directory=output_directory
    )

    # Convert to int16
    audio_int16 = (original_audio_samples * np.iinfo(audio_integer_type).max).astype(audio_integer_type)
    assert (np.max(audio_int16) >= 1.0)

    # Compare to audio file
    audio_int16_loaded = np.fromfile(
        file=os.path.join("tools", "audio", "{}_audio.edge".format(audio_basename)),
        dtype=audio_integer_type,
    )
    assert (len(audio_int16_loaded) == len(audio_int16))

    total_big_errors = 0
    for iterator in range(0, len(audio_int16)):
        if audio_int16[iterator] != audio_int16_loaded[iterator]:
            absolute_error = abs(audio_int16[iterator] - audio_int16_loaded[iterator])
            if absolute_error > absolute_error_tolerance_for_integer:
                print("Big Mismatch at iterator: {}".format(iterator))
                print("From audio: {}, from loaded: {}, error: {}".format(
                    audio_int16[iterator],
                    audio_int16_loaded[iterator],
                    absolute_error
                ))
                total_big_errors = total_big_errors + 1

    # Get the buffers
    power_spectrum, mel_spec, scaled_mel_spec_uint = generate_mel_spectrogram_and_buffers(
        audio_dsp_c_lib=audio_dsp_c_lib,
        audio_int16=audio_int16,
        sample_rate=sample_rate,
        n_fft=n_fft,
        hop_length=hop_length,
        max_num_frames=max_num_frames,
        n_mels=n_mels,
        fmax=fmax,
    )
    assert (power_spectrum.shape == power_spectrum_expected_shape)
    assert (power_spectrum.dtype == np.float32)

    assert (len(mel_spec) == mel_spectrogram_expected_shape[0] * mel_spectrogram_expected_shape[1])
    assert (mel_spec.dtype == np.float32)

    assert (scaled_mel_spec_uint.dtype == np.uint8)
    assert (scaled_mel_spec_uint.shape == mel_spec.shape)
    assert (len(scaled_mel_spec_uint) == mel_spectrogram_expected_shape[0] * mel_spectrogram_expected_shape[1])

    # Save power spectrum as header file
    power_spectrum_flattened = np.reshape(
        power_spectrum,
        newshape=-1,
    )
    power_spectrum_cog_dict = eb.get_empty_cog_dict()
    power_spectrum_cog_dict["file_prefix"] = "{}_POWER_SPECTRUM".format(audio_basename.upper())
    power_spectrum_cog_dict["data_array"] = power_spectrum_flattened
    power_spectrum_cog_dict["comment_strings"] = ""
    eb.export_ndarray(
        target_filepath=os.path.join(
            output_directory,
            "{}_power_spectrum.h".format(audio_basename.lower())
        ),
        ndarray_type="float",
        cog_dict=power_spectrum_cog_dict,
    )

    # Save power spectrum plot
    save_plots(
        target_array=power_spectrum,
        target_filepath=os.path.join(output_directory, "computed_power_spectrum.png"),
    )

    # Save mel spectrogram as header file
    mel_spec_cog_dict = eb.get_empty_cog_dict()
    mel_spec_cog_dict["file_prefix"] = "{}_MEL_SPEC".format(audio_basename.upper())
    mel_spec_cog_dict["data_array"] = mel_spec
    mel_spec_cog_dict["comment_strings"] = ""
    eb.export_ndarray(
        target_filepath=os.path.join(
            output_directory,
            "{}_mel_spec.h".format(audio_basename.lower())
        ),
        ndarray_type="float",
        cog_dict=mel_spec_cog_dict,
    )

    # Save unflattened mel spectrogram plot
    mel_spec_unflattened = np.reshape(
        mel_spec,
        newshape=mel_spectrogram_expected_shape,
    )
    save_plots(
        target_array=mel_spec_unflattened,
        target_filepath=os.path.join(output_directory, "computed_mel_spec.png"),
    )

    # Save scaled mel spectrogram as header file
    scaled_mel_spec_cog_dict = eb.get_empty_cog_dict()
    scaled_mel_spec_cog_dict["file_prefix"] = "{}_SCALED_MEL_SPEC".format(audio_basename.upper())
    scaled_mel_spec_cog_dict["data_array"] = scaled_mel_spec_uint
    scaled_mel_spec_cog_dict["comment_strings"] = ""
    eb.export_ndarray(
        target_filepath=os.path.join(
            output_directory,
            "{}_scaled_mel_spec.h".format(audio_basename.lower())
        ),
        ndarray_type="uint8",
        cog_dict=scaled_mel_spec_cog_dict,
    )

    # Save unflattened scaled mel spectrogram (as uint8) plot
    scaled_mel_spec_uint_unflattened = np.reshape(
        scaled_mel_spec_uint,
        newshape=mel_spectrogram_expected_shape,
    )
    save_plots(
        target_array=scaled_mel_spec_uint_unflattened,
        target_filepath=os.path.join(output_directory, "computed_scaled_mel_spec.png"),
    )

    # Compare to mel file
    loaded_mel_spec_uint_ = np.fromfile(
        file=os.path.join("tools", "audio", "{}_mel_uint.edge".format(audio_basename)),
        dtype=np.uint8,
    )

    assert (len(scaled_mel_spec_uint) == len(loaded_mel_spec_uint_))

    total_big_errors = 0
    for iterator in range(0, len(loaded_mel_spec_uint_)):
        if scaled_mel_spec_uint[iterator] != loaded_mel_spec_uint_[iterator]:
            absolute_error = abs(scaled_mel_spec_uint[iterator] - loaded_mel_spec_uint_[iterator])
            if absolute_error > absolute_error_tolerance_for_integer:
                print("Big Mismatch for scaled mel spec at iterator: {}".format(iterator))
                print("From mel: {}, from loaded: {}, error: {}".format(
                    scaled_mel_spec_uint[iterator],
                    loaded_mel_spec_uint_[iterator],
                    absolute_error
                ))
                total_big_errors = total_big_errors + 1

    # Save loaded mel spectrogram as a plot
    loaded_mel_spec_uint_unflattened = np.reshape(
        loaded_mel_spec_uint_,
        newshape=(n_mels, max_num_frames),
    )
    save_plots(
        loaded_mel_spec_uint_unflattened,
        os.path.join(output_directory, "loaded_scaled_mel_spec.png"),
    )

    print("Done")


if __name__ == '__main__':
    generate_edge_unit_tests()
