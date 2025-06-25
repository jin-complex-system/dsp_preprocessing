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
    constexpr uint16_t n_ffts[] = {1024u};
    constexpr uint16_t sample_rates[] = {22048u, 44100u};
    constexpr uint16_t max_frequencies[] = {0u, 8000u};
    constexpr uint16_t n_mels[] = {32u, 64u};

    for (const auto n_fft : n_ffts) {
        for (const auto sample_rate : sample_rates) {
            for (const auto n_mel : n_mels) {
                for (const auto max_frequency : max_frequencies) {
                    float mel_centre_freq_float_buffer[n_mel + 1] = {};
                    uint16_t mel_centre_freq_next_bin_buffer[n_mel - 1] = {};
                    uint16_t mel_centre_freq_prev_bin_buffer[n_mel - 1] = {};
                    float mel_freq_weights_buffer[n_mel - 1] = {};

                    compute_mel_spectrogram_bins(
                        n_mel,
                        n_fft,
                        sample_rate,
                        max_frequency,
                        mel_centre_freq_float_buffer,
                        mel_centre_freq_next_bin_buffer,
                        mel_centre_freq_prev_bin_buffer,
                        mel_freq_weights_buffer
                    );

                    /*
                    /// Compare against precomputed values
                    {
                        const float* precomputed_mel_centre_freq_float_buffer = nullptr;
                        const uint16_t* precomputed_mel_centre_freq_next_bin_buffer = nullptr;
                        const uint16_t* precomputed_mel_centre_freq_prev_bin_buffer = nullptr;
                        const float* precomputed_mel_freq_weights_buffer = nullptr;

                        const bool found_precomputed_values =
                        get_mel_spectrogram_precomputed_values(
                            n_mel,
                            n_fft,
                            sample_rate,
                            max_frequency,
                            &precomputed_mel_centre_freq_float_buffer,
                            &precomputed_mel_centre_freq_next_bin_buffer,
                            &precomputed_mel_centre_freq_prev_bin_buffer,
                            &precomputed_mel_freq_weights_buffer
                        );

                        EXPECT_TRUE(found_precomputed_values);

                        int result;

                        result = memcmp(
                            precomputed_mel_centre_freq_float_buffer,
                            &mel_centre_freq_float_buffer,
                            (n_mel + 1) * sizeof(float));
                        EXPECT_EQ(result, 0);

                        result = memcmp(
                            precomputed_mel_centre_freq_next_bin_buffer,
                            &mel_centre_freq_next_bin_buffer,
                            (n_mel - 1) * sizeof(uint16_t));
                        EXPECT_EQ(result, 0);

                        result = memcmp(
                            precomputed_mel_centre_freq_prev_bin_buffer,
                            &mel_centre_freq_prev_bin_buffer,
                            (n_mel - 1) * sizeof(uint16_t));
                        EXPECT_EQ(result, 0);

                        result = memcmp(
                            precomputed_mel_freq_weights_buffer,
                            &mel_freq_weights_buffer,
                            (n_mel - 1) * sizeof(float));
                        EXPECT_EQ(result, 0);
                    }
                    */
                }
            }
        }
    }
}
