import numpy
from cogapp import Cog


def get_empty_cog_dict():
    """
    Get empty cog dict
    :return: empty dictionary of expected values
    """

    return {
        "file_prefix": "my_float",
        "data_array": None,
        "comment_strings": "/n"
    }


class MelPrecomputeStructure:
    n_fft = 0
    n_mel = 0
    sample_rate = 0


def get_empty_mel_precompute_cog_dict():
    """
    Get empty cog dict for mel_get_precompute
    :return: empty dictionary of expected values
    """

    return {
        "data_structures": list()
    }


def export_ndarray(
        target_filepath,
        ndarray_type,
        cog_dict):
    """

    :param target_filepath: export filepath
    :param ndarray_type: type of ndarray; must be either "float" or "uint16"
    :param cog_dict: filled dict that matches to get_empty_cog_dict()
    :return:
    """
    # Libraries
    import os
    from pathlib import Path

    # Check parameters
    assert (len(target_filepath) > 0)
    assert (ndarray_type is "float" or ndarray_type is "uint16")

    # Create target directory, if it does not exist
    os.makedirs(name=os.path.dirname(target_filepath), exist_ok=True)

    # Note that template folder is always relative to this file
    template_filepath = os.path.join(
        Path(__file__).parent,
        "templates",
        "np_array_header_{}.h".format(ndarray_type)
    )
    # print(template_filepath)

    my_cog = Cog()
    my_cog.options.bDeleteCode = True
    my_cog.processFile(
        fIn=template_filepath,
        fOut=target_filepath,
        globals=cog_dict)


def export_mel_precompute_c_file(
        target_filepath,
        mel_precompute_cog_dict):
    """

    :param target_filepath:
    :param mel_precompute_cog_dict: filled dict that matches to get_empty_mel_precompute_cog_dict()
    :return:
    """
    # Libraries
    import os
    from pathlib import Path

    # Check parameters
    assert (len(target_filepath) > 0)
    assert (len(mel_precompute_cog_dict["data_structures"]) > 0)

    # Create target directory, if it does not exist
    os.makedirs(name=os.path.dirname(target_filepath), exist_ok=True)

    # Note that template folder is always relative to this file
    template_filepath = os.path.join(
        Path(__file__).parent,
        "templates",
        "_template_mel_get_precompute.c"
    )
    # print(template_filepath)

    my_cog = Cog()
    my_cog.options.bDeleteCode = True
    my_cog.processFile(
        fIn=template_filepath,
        fOut=target_filepath,
        globals=mel_precompute_cog_dict)
