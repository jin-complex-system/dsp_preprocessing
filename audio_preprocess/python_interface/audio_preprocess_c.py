import ctypes
import numpy as np


class audio_preprocess_c:
    libaudiopreprocess = None

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
        :return:
        """

        assert (library_path is not None and len(library_path) > 0)
        self.libaudiopreprocess = ctypes.CDLL(library_path)

