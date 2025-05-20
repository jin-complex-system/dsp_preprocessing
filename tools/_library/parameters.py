import numpy as np


class DefaultParameters:
    def __init__(self):
        self.output_directory = "_output_processed"

        self.n_ffts = [1024]
        self.scaling_factor_float = 1.0 / np.iinfo(np.int16).max  # Max of int16_T

        self.n_mels = [64]
        self.sample_rates = [22048, 44100]
