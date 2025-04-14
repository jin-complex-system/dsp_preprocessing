#include <gtest/gtest.h>
#include <mel_spectrogram.h>
#include <cmath>

static constexpr double
FREQUENCIES[] = {
    1, 100, 200, 400, 800, 1000, 2000, 4000, 8000, 16000, 32000, 44100, 48000
};

static const double
MINIMUM_FREQUENCY_SLANEY = 1000.0; // Frequency at 15.0 mel

static const double
MINIMUM_MEL_SLANEY = 15.0; // Mel at 1000 Hz

TEST(MelPrecomputed, ConversionHtkMelToFreq) {
    constexpr double
    HTK_FREQ_TO_MEL_COMPUTED_ERROR_TOLERANCE = 9.5 * 1e-3;

    for (const double original_frequency : FREQUENCIES) {
        const auto expected_mel_htk = 2595.0 * log10(1 + original_frequency / 700.0);
        const float computed_mel_htk = convert_frequency_to_mel_htk((float)original_frequency);

        EXPECT_NEAR(
            expected_mel_htk,
            computed_mel_htk,
            HTK_FREQ_TO_MEL_COMPUTED_ERROR_TOLERANCE);
    }
}

TEST(MelPrecomputed, ConversionHtkFreqToMel) {
    constexpr double
    HTK_MEL_TO_FREQ_COMPUTED_ERROR_TOLERANCE = 1e-2;

    for (const double original_frequency : FREQUENCIES) {
        const auto MEL_HTK = 2595.0 * log10(1 + original_frequency / 700.0);

        const auto freq_htk = 700.0 * (pow(10.0, MEL_HTK / 2595.0) - 1.0);
        const float computed_freq_htk = convert_mel_to_frequency_htk((float)MEL_HTK);

        EXPECT_NEAR(
            freq_htk,
            computed_freq_htk,
            HTK_MEL_TO_FREQ_COMPUTED_ERROR_TOLERANCE);
    }
}

TEST(MelPrecomputed, ConversionSlaneyFreqToMel) {
    constexpr double
    SLANEY_FREQ_TO_MEL_COMPUTED_ERROR_TOLERANCE = 9.5 * 1e-3;

    for (const double original_frequency : FREQUENCIES) {
        const float computed_mel_slaney = convert_frequency_to_mel_slaney((float)original_frequency);

        double expected_mel_slaney;
        /// Log scale
        if (original_frequency < MINIMUM_FREQUENCY_SLANEY) {
            expected_mel_slaney = 0.015 * original_frequency;
        }
        /// Linear scale
        else {
            expected_mel_slaney =
                MINIMUM_MEL_SLANEY +
                    27.0 * log10 (original_frequency / MINIMUM_FREQUENCY_SLANEY) / log10(6.4);
        }

        EXPECT_NEAR(
            expected_mel_slaney,
            computed_mel_slaney,
            SLANEY_FREQ_TO_MEL_COMPUTED_ERROR_TOLERANCE);
    }
}

TEST(MelPrecomputed, ConversionSlaneyMelToFreq) {
    constexpr double
    SLANEY_MEL_TO_FREQ_COMPUTED_ERROR_TOLERANCE = 9 * 1e-1;

    for (const double original_frequency : FREQUENCIES) {
        double mel_slaney;
        /// Log scale
        if (original_frequency < MINIMUM_FREQUENCY_SLANEY) {
            mel_slaney = 0.015 * original_frequency;
        }
        /// Linear scale
        else {
            mel_slaney =
                MINIMUM_MEL_SLANEY +
                    27.0 * log10 (original_frequency / MINIMUM_FREQUENCY_SLANEY) / log10(6.4);
        }
        const float computed_freq_slaney = convert_mel_to_frequency_slaney((float)mel_slaney);

        EXPECT_NEAR(
            original_frequency,
            computed_freq_slaney,
            SLANEY_MEL_TO_FREQ_COMPUTED_ERROR_TOLERANCE);
    }
}

TEST(MelPrecomputed, ComputeMelBins) {
    // TODO: Implement this unit test
}
