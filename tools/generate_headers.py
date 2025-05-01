import os
import sys
import _library.parameters as parameters
from _library.generate_headers_audio_dsp import GenerateAudioDSP


class MyParameters(parameters.DefaultParameters):
    def __init__(self):

        # Load default parameters
        super().__init__()

        # Custom parameters
        # If needed, update your parameters here

def generate_headers_main():
    BINARY_LOCATION = sys.argv[1]
    AUDIO_DSP_PYTHON_INTERFACE = sys.argv[2]

    params = MyParameters()

    # Create necessary output directory
    assert (len(params.output_directory) > 0)
    os.makedirs(params.output_directory, exist_ok=True)
    print("Precomputed headers will be generated to {}".format(params.output_directory))

    # Audio DSP
    my_audio_dsp_gen = GenerateAudioDSP(
        audio_dsp_python_interface_filepath=AUDIO_DSP_PYTHON_INTERFACE,
        audio_dsp_library_binary_filepath=BINARY_LOCATION,
    )
    my_audio_dsp_gen.generate_headers(params=params)

    print("Done")


if __name__ == '__main__':
    generate_headers_main()
