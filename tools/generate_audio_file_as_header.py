import librosa
import _library.export_buffer as eb
import os
import numpy as np


def _generate_audio_file_as_header(
        audio_filepath,
        audio_type,
        output_directory):
    """
    Generate audio file as a header
    :param audio_filepath:
    :param audio_type:
    :param output_directory:
    :return:
    """
    # Check parameters
    assert (audio_type is np.int16)  # At the moment, only support np.int16
    assert (len(audio_filepath) > 0)
    assert (len(output_directory) > 0)

    # Create output directory if it does not exist before
    os.makedirs(output_directory, exist_ok=True)

    # Grab audio filename and prefix
    audio_filename = os.path.basename(audio_filepath).strip()
    audio_filename_with_no_ext = audio_filename.split('.')[0].strip()
    audio_prefix_lowercase = audio_filename_with_no_ext.lower()
    audio_prefix_uppercase = audio_filename_with_no_ext.upper()

    # Load the file as float
    # Note that we are re-sampling to 44.1 kHz and forcing used of mono
    samples_float, sample_rate = librosa.load(
        audio_filepath,
        sr=44100,
        mono=True,
        dtype=np.float32,
    )

    # Forcefully load samples back as audio_type
    scaling_factor_float_to_audio_type = np.iinfo(audio_type).max
    samples = np.array(
        [int(s * scaling_factor_float_to_audio_type) for s in samples_float],
        dtype=audio_type,
    )

    duration = librosa.get_duration(y=samples_float, sr=sample_rate)

    # Prepare comments
    audio_comment_string = \
        ("#define {}_SAMPLE_RATE {}u\n".format(audio_prefix_uppercase, int(sample_rate)) +
         "#define {}_DURATION {}u\n".format(audio_prefix_uppercase, int(duration)))

    # Prepare dict
    audio_cog_dict = eb.get_empty_cog_dict()
    audio_cog_dict["file_prefix"] = "{}".format(audio_prefix_lowercase)
    audio_cog_dict["data_array"] = samples
    audio_cog_dict["comment_strings"] = audio_comment_string

    # Export samples
    eb.export_ndarray(
        target_filepath=os.path.join(
            output_directory,
            "{}.h".format(audio_filename_with_no_ext)
        ),
        ndarray_type="int16",
        cog_dict=audio_cog_dict,
    )


def generate_audio_file_as_header_main():
    # Constants; change as needed
    audio_filepaths = [
        "examples/human_voice.wav",
    ]
    output_directory = "_audio_headers"
    audio_type = np.int16

    for audio_filepath in audio_filepaths:
        _generate_audio_file_as_header(
            audio_filepath=audio_filepath,
            audio_type=audio_type,
            output_directory=output_directory,)
    print("Done")


if __name__ == '__main__':
    generate_audio_file_as_header_main()