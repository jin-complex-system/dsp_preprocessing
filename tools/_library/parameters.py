import numpy as np


class DefaultParameters:
    def __init__(self):
        self.output_directory = "_output_processed"

        self.n_ffts = [1024, 2048]
        self.scaling_factor_float = 1.0 / np.iinfo(np.int16).max  # Max of int16_t

        self.n_mels = [32, 64]
        self.sample_rates = [22048, 44100]
        self.max_frequencies = [
            0,  # use sample_rate / 2
            8000,
        ]
