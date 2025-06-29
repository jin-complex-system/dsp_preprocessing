import unittest
from python_interface.audio_dsp_c import audio_dsp_c
import sys
import glob
import numpy as np
import librosa

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
audio_dsp_c_lib = audio_dsp_c(library_path=LIBRARY_PATH)

FREQ_TO_MEL_CONVERSION_ERROR_DELTA = 1
MEL_TO_FREQ_CONVERSION_ERROR_DELTA = 1

MIN_FREQUENCY = 0
MAX_FREQUENCY = 48000

class AudioDSP_MelCompute_PythonTestCase(unittest.TestCase):
    def test_convert_freq_to_mel(self):
        htk_values = [True, False]

        frequencies = np.linspace(MIN_FREQUENCY, MAX_FREQUENCY).astype(np.uint32)
        for use_htk in htk_values:
            for iterator in range(0, len(frequencies)):
                frequency = frequencies[iterator]
                mel_librosa = librosa.hz_to_mel(frequency, htk=use_htk)

                # Compare frequency input
                mel_from_freq_integer = audio_dsp_c_lib.convert_frequency_to_mel(
                    frequency=int(frequency), htk=use_htk)
                mel_from_freq_float = audio_dsp_c_lib.convert_frequency_to_mel(
                    frequency=float(frequency), htk=use_htk)
                self.assertEqual(mel_from_freq_integer, mel_from_freq_float)

                # Compare against mel computed from librosa
                self.assertAlmostEqual(
                    first=mel_librosa,
                    second=mel_from_freq_float,
                    delta=FREQ_TO_MEL_CONVERSION_ERROR_DELTA,
                    msg="\r\nHTK={} and Freq {}, librosa {} vs computed float {}".format(
                        use_htk,
                        frequency,
                        mel_librosa,
                        mel_from_freq_float
                    )
                )

    def test_convert_mel_to_freq(self):
        htk_values = [True, False]

        frequencies = np.linspace(MIN_FREQUENCY, MAX_FREQUENCY).astype(np.uint32)
        for use_htk in htk_values:
            for iterator in range(0, len(frequencies)):
                original_frequency = frequencies[iterator]
                mel_librosa = librosa.hz_to_mel(original_frequency, htk=use_htk)

                freq_from_mel_float = audio_dsp_c_lib.convert_mel_to_frequency(
                    mel=float(mel_librosa), htk=use_htk)

                # Compare against original frequency
                self.assertAlmostEqual(
                    first=original_frequency,
                    second=freq_from_mel_float,
                    delta=MEL_TO_FREQ_CONVERSION_ERROR_DELTA,
                    msg="\r\nHTK={} and mel {}, original {} vs computed float {}".format(
                        use_htk,
                        mel_librosa,
                        original_frequency,
                        freq_from_mel_float
                    )
                )

    def test_compute_mel_spectrogram_bins(self):
        n_ffts = [1024]
        sample_rates = [22048, 44100]
        max_frequencies = [0, 8000]
        n_mels = [32, 64]

        for n_fft in n_ffts:
            for sample_rate in sample_rates:
                for n_mel in n_mels:
                    for max_frequency in max_frequencies:
                        # Compute spectrogram constants
                        m_float, m_prev, m_next, m_weights = audio_dsp_c_lib.compute_mel_spectrogram_bins(
                            n_mel_uint16=n_mel,
                            n_fft_uint16=n_fft,
                            sample_rate_uint16=sample_rate,
                            max_frequency_uint16=max_frequency,
                        )
                         # TODO: Compare to librosa


if __name__ == '__main__':
    unittest.main()
