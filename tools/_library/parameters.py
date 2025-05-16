class DefaultParameters:
    def __init__(self):
        self.output_directory = "_output_processed"

        self.n_ffts = [512, 1024, 2048]
        self.scaling_factor_float = 1.0

        self.n_mels = [32, 64, 128]
        self.sample_rates = [22048]
