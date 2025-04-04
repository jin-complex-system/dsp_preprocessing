#include <gtest/gtest.h>
#include <power_spectrum.h>
#include <precomputed_hann/hann_window_with_scale_1024.h>

#include <limits>
#include <test_audio/audio_as_integer.h>
#include <test_audio/audio_as_float.h>
#include <test_audio/power_spectrum.h>
#include <power_spectrum_unit_test/sinewave_input.h>
#include <power_spectrum_unit_test/power_spectrum_unit_test.h>
#include <power_spectrum_unit_test/sinewave_rfft_decibel_magnitude.h>

constexpr auto
MAX_AUDIO_DATA_TYPE = std::numeric_limits<audio_data_type>::max();

constexpr double
NORMALISATION_FLOAT_ERROR_TOLERANCE = 1e-4;

constexpr double
POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE = 1.0;

constexpr uint32_t
COMPUTED_NUM_FRAMES = (
    AUDIO_INTEGER_BUFFER_LENGTH / PRECOMPUTED_POWER_SPECTRUM_HOP_LENGTH -
    PRECOMPUTED_POWER_SPECTRUM_N_FFT / PRECOMPUTED_POWER_SPECTRUM_HOP_LENGTH) + 1;

TEST(PowerSpectrum, NormaliseIntegerToFloat) {
    /// Check parameters
    {
        assert(AUDIO_FLOAT_BUFFER_LENGTH == AUDIO_INTEGER_BUFFER_LENGTH);
    }

    /// Normalise integer to float
    {
        for (uint32_t iterator = 0; iterator < AUDIO_INTEGER_BUFFER_LENGTH; iterator++) {
            const float normalised_value = (float)AUDIO_INTEGER_BUFFER[iterator] / MAX_AUDIO_DATA_TYPE;

            EXPECT_NEAR(
                normalised_value,
                AUDIO_FLOAT_BUFFER[iterator],
                NORMALISATION_FLOAT_ERROR_TOLERANCE);
        }
    }
}

TEST(PowerSpectrum, SinewaveNoScratchBuffer) {
    /// Check parameters
    {
        assert(
            SINEWAVE_INPUT_BUFFER_LENGTH /2 + 1 == POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH &&
            POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH == SINEWAVE_RFFT_DECIBEL_MAGNITUDE_BUFFER_LENGTH);
        assert(SINEWAVE_INPUT_BUFFER_LENGTH == HANN_WINDOW_SCALED_1024_LENGTH);
    }
    constexpr uint32_t OUTPUT_BUFFER_LENGTH = SINEWAVE_INPUT_BUFFER_LENGTH * 2;

    /// No scratch buffer with manual init and deinit
    {
        initialise_power_spectrum(SINEWAVE_INPUT_BUFFER_LENGTH);

        float output_buffer[OUTPUT_BUFFER_LENGTH];

        const bool
        power_threshold_met = compute_power_spectrum_audio_samples(
            SINEWAVE_INPUT_BUFFER,
            SINEWAVE_INPUT_BUFFER_LENGTH,
            output_buffer,
            OUTPUT_BUFFER_LENGTH,
            nullptr,
            0,
            0.0f,
            HANN_WINDOW_SCALED_1024,
            HANN_WINDOW_SCALED_1024_LENGTH);
        EXPECT_TRUE(power_threshold_met);

        for (uint32_t power_spectrum_iterator = 0; power_spectrum_iterator < POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH; power_spectrum_iterator++) {
            EXPECT_NEAR(
                POWER_SPECTRUM_UNIT_TEST_BUFFER[power_spectrum_iterator],
                output_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);

            EXPECT_NEAR(
                SINEWAVE_RFFT_DECIBEL_MAGNITUDE_BUFFER[power_spectrum_iterator],
                output_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);
        }

        deinit_power_spectrum();
    }

    /// No scratch buffer calling compute_power_spectrum_audio_samples_direct()
    {
        float output_buffer[OUTPUT_BUFFER_LENGTH];

        compute_power_spectrum_audio_samples_direct(
            SINEWAVE_INPUT_BUFFER,
            SINEWAVE_INPUT_BUFFER_LENGTH,
            output_buffer,
            OUTPUT_BUFFER_LENGTH,
            HANN_WINDOW_SCALED_1024,
            HANN_WINDOW_SCALED_1024_LENGTH);

        for (uint32_t power_spectrum_iterator = 0; power_spectrum_iterator < POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH; power_spectrum_iterator++) {
            EXPECT_NEAR(
                POWER_SPECTRUM_UNIT_TEST_BUFFER[power_spectrum_iterator],
                output_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);
            EXPECT_NEAR(
                SINEWAVE_RFFT_DECIBEL_MAGNITUDE_BUFFER[power_spectrum_iterator],
                output_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);
        }
    }
}

TEST(PowerSpectrum, SinewaveWithScratchBuffer) {
    /// Check parameters
    {
        assert(
            SINEWAVE_INPUT_BUFFER_LENGTH /2 + 1 == POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH &&
            POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH == SINEWAVE_RFFT_DECIBEL_MAGNITUDE_BUFFER_LENGTH);
        assert(SINEWAVE_INPUT_BUFFER_LENGTH == HANN_WINDOW_SCALED_1024_LENGTH);
    }

    constexpr uint32_t OUTPUT_BUFFER_LENGTH = SINEWAVE_INPUT_BUFFER_LENGTH / 2 + 1;
    constexpr uint32_t SCRATCH_BUFFER_LENGTH = SINEWAVE_INPUT_BUFFER_LENGTH * 2;

    /// Scratch buffer with manual init and deinit
    {
        initialise_power_spectrum(SINEWAVE_INPUT_BUFFER_LENGTH);

        float output_buffer[OUTPUT_BUFFER_LENGTH];
        float scratch_buffer[SCRATCH_BUFFER_LENGTH];

        const bool
        power_threshold_met = compute_power_spectrum_audio_samples(
            SINEWAVE_INPUT_BUFFER,
            SINEWAVE_INPUT_BUFFER_LENGTH,
            output_buffer,
            OUTPUT_BUFFER_LENGTH,
            scratch_buffer,
            SCRATCH_BUFFER_LENGTH,
            0.0f,
            HANN_WINDOW_SCALED_1024,
            HANN_WINDOW_SCALED_1024_LENGTH);
        EXPECT_TRUE(power_threshold_met);

        for (uint32_t power_spectrum_iterator = 0; power_spectrum_iterator < POWER_SPECTRUM_UNIT_TEST_BUFFER_LENGTH; power_spectrum_iterator++) {
            EXPECT_NEAR(
                POWER_SPECTRUM_UNIT_TEST_BUFFER[power_spectrum_iterator],
                output_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);

            EXPECT_NEAR(
                SINEWAVE_RFFT_DECIBEL_MAGNITUDE_BUFFER[power_spectrum_iterator],
                output_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);
        }

        deinit_power_spectrum();
    }
}

// TODO: Update precomputed constants
/*
TEST(PowerSpectrum, PrecomputedPowerSpectrumNoScratchBuffer) {
    /// Check parameters
    {
        assert(AUDIO_INTEGER_BUFFER_LENGTH == AUDIO_FLOAT_BUFFER_LENGTH);
        assert(PRECOMPUTED_POWER_SPECTRUM_HOP_LENGTH == PRECOMPUTED_POWER_SPECTRUM_N_FFT);
        assert(PRECOMPUTED_POWER_SPECTRUM_N_FFT /2 + 1 == PRECOMPUTED_POWER_SPECTRUM_BINS);
        assert(COMPUTED_NUM_FRAMES == PRECOMPUTED_POWER_SPECTRUM_NUM_FRAMES);
        assert(sizeof(AUDIO_INTEGER_BUFFER) / (AUDIO_INTEGER_BUFFER_LENGTH) == sizeof(audio_data_type));
        assert(HANN_WINDOW_SCALED_1024_LENGTH == PRECOMPUTED_POWER_SPECTRUM_N_FFT);
    }

    initialise_power_spectrum(PRECOMPUTED_POWER_SPECTRUM_N_FFT);

    /// No scratch buffer
    {
        for (uint32_t frame_iterator = 0; frame_iterator < COMPUTED_NUM_FRAMES; frame_iterator++) {
            const auto current_audio_pointer = (audio_data_type*)&AUDIO_INTEGER_BUFFER[frame_iterator * AUDIO_INTEGER_BUFFER_LENGTH];

            constexpr uint32_t power_spectrum_buffer_length = PRECOMPUTED_POWER_SPECTRUM_N_FFT * 2;
            float power_spectrum_buffer[power_spectrum_buffer_length];
            assert(power_spectrum_buffer_length > PRECOMPUTED_POWER_SPECTRUM_BINS);

            const bool
            power_threshold_met = compute_power_spectrum_audio_samples(
                current_audio_pointer,
                PRECOMPUTED_POWER_SPECTRUM_N_FFT,
                power_spectrum_buffer,
                power_spectrum_buffer_length,
                nullptr,
                0,
                0.0f,
                HANN_WINDOW_SCALED_1024,
                HANN_WINDOW_SCALED_1024_LENGTH
            );
            EXPECT_TRUE(power_threshold_met);

            for (uint32_t power_spectrum_iterator = 0; power_spectrum_iterator < PRECOMPUTED_POWER_SPECTRUM_BINS; power_spectrum_iterator++) {
                const uint32_t buffer_iterator = power_spectrum_iterator + PRECOMPUTED_POWER_SPECTRUM_BINS * frame_iterator;
                assert(buffer_iterator < PRECOMPUTED_POWER_SPECTRUM_BUFFER_LENGTH);

                const float
                expected_power_spectrum_value = PRECOMPUTED_POWER_SPECTRUM_BUFFER[buffer_iterator];
                EXPECT_NEAR(
                    expected_power_spectrum_value,
                    power_spectrum_buffer[power_spectrum_iterator],
                    POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);
            }
        }
    }
}

TEST(PowerSpectrum, PrecomputedPowerSpectrumWithScratchBuffer) {
    /// Check parameters
    {
        assert(AUDIO_INTEGER_BUFFER_LENGTH == AUDIO_FLOAT_BUFFER_LENGTH);
        assert(PRECOMPUTED_POWER_SPECTRUM_HOP_LENGTH == PRECOMPUTED_POWER_SPECTRUM_N_FFT);
        assert(PRECOMPUTED_POWER_SPECTRUM_N_FFT /2 + 1 == PRECOMPUTED_POWER_SPECTRUM_BINS);
        assert(COMPUTED_NUM_FRAMES == PRECOMPUTED_POWER_SPECTRUM_NUM_FRAMES);
        assert(sizeof(AUDIO_INTEGER_BUFFER) / (AUDIO_INTEGER_BUFFER_LENGTH) == sizeof(audio_data_type));
        assert(HANN_WINDOW_SCALED_1024_LENGTH == PRECOMPUTED_POWER_SPECTRUM_N_FFT);
    }

    initialise_power_spectrum(PRECOMPUTED_POWER_SPECTRUM_N_FFT);

    /// With scratch buffer
    {
        for (uint32_t frame_iterator = 0; frame_iterator < COMPUTED_NUM_FRAMES; frame_iterator++) {
            const auto current_audio_pointer = (audio_data_type*)&AUDIO_INTEGER_BUFFER[frame_iterator * AUDIO_INTEGER_BUFFER_LENGTH];

            constexpr uint32_t power_spectrum_buffer_length = PRECOMPUTED_POWER_SPECTRUM_N_FFT / 2 + 1;
            float power_spectrum_buffer[power_spectrum_buffer_length];

            constexpr uint32_t scratch_buffer_length = PRECOMPUTED_POWER_SPECTRUM_N_FFT * 2;
            float scratch_buffer[scratch_buffer_length];

            const bool
            power_threshold_met = compute_power_spectrum_audio_samples(
                current_audio_pointer,
                PRECOMPUTED_POWER_SPECTRUM_N_FFT,
                power_spectrum_buffer,
                power_spectrum_buffer_length,
                scratch_buffer,
                scratch_buffer_length,
                0.0f,
                HANN_WINDOW_SCALED_1024,
                HANN_WINDOW_SCALED_1024_LENGTH
            );
            EXPECT_TRUE(power_threshold_met);

            for (uint32_t power_spectrum_iterator = 0; power_spectrum_iterator < PRECOMPUTED_POWER_SPECTRUM_BINS; power_spectrum_iterator++) {
                const uint32_t buffer_iterator = power_spectrum_iterator + PRECOMPUTED_POWER_SPECTRUM_BINS * frame_iterator;
                assert(buffer_iterator < PRECOMPUTED_POWER_SPECTRUM_BUFFER_LENGTH);

                const float
                expected_power_spectrum_value = PRECOMPUTED_POWER_SPECTRUM_BUFFER[buffer_iterator];
                EXPECT_NEAR(
                    expected_power_spectrum_value,
                    power_spectrum_buffer[power_spectrum_iterator],
                    POWER_SPECTRUM_FLOAT_ERROR_TOLERANCE);
            }
        }
    }
}
*/
