#include <gtest/gtest.h>
#include <mel_spectrogram.h>

#include <limits>
#include <mel_spectrogram_unit_test/fft_unit_input.h>
#include <mel_spectrogram_unit_test/mel_spectrogram_unit_test.h>
#include <test_audio/mel_spectrogram.h>

constexpr double
MEL_COMPUTED_ERROR_TOLERANCE = 1e-3;

TEST(MelPrecomputed, UnitFilterDistribution_32) {
    /// Check parameters
    {
        assert(FFT_UNIT_INPUT_N_FFT > 0);
        assert(MEL_SPECTROGRAM_N_MELS == PRECOMPUTED_MEL_SPECTROGRAM_N_MELS);
        assert(FFT_UNIT_INPUT_NUM_FRAMES == MEL_SPECTROGRAM_NUM_FRAMES && FFT_UNIT_INPUT_NUM_FRAMES == 1);
    }

    float mel_spectrogram_buffer[MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH] = {};

    /// Compute mel spectrogram
    compute_power_spectrum_into_mel_spectrogram(
        FFT_UNIT_INPUT_BUFFER,
        FFT_UNIT_INPUT_BUFFER_LENGTH,
        mel_spectrogram_buffer,
        MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH);

    /// Check against mel spectrogram
    for (uint32_t mel_bin_iterator = 0; mel_bin_iterator < MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH; mel_bin_iterator++) {
        EXPECT_NEAR(
            MEL_SPECTROGRAM_UNIT_TEST_BUFFER[mel_bin_iterator],
            mel_spectrogram_buffer[mel_bin_iterator],
            MEL_COMPUTED_ERROR_TOLERANCE);
    }
}
