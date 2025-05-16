import numpy
from cogapp import Cog

def get_empty_cog_dict():
    """
    Get empty cog dict
    :return: empty dictionary of expected values
    """

    return {
        "file_prefix": "my_float",
        "float_array": None,
        "comment_strings": "/n"
    }

def export_ndarray(
        target_filepath,
        cog_dict):
    """

    :param target_filepath: export filepath
    :param cog_dict: filled dict that matches to get_empty_cog_dict()
    :return:
    """

    # Check parameters
    assert (len(target_filepath) > 0)
    import os
    from pathlib import Path

    # Note that template folder is always relative to this file
    template_filepath = os.path.join(
        Path(__file__).parent,
        "templates",
        "np_array_header.h"
    )
    # print(template_filepath)

    myCog = Cog()
    myCog.options.bDeleteCode = True
    myCog.processFile(
        fIn=template_filepath,
        fOut=target_filepath,
        globals=cog_dict)
