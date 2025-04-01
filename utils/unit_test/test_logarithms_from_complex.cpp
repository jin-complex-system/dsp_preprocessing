#include <gtest/gtest.h>

#include <convert_complex.h>
#include <cassert>
#include <cmath>

#include <FFT_audio/rfft_output.h>
#include <FFT_audio/rfft_log_magnitude.h>
#include <FFT_audio/rfft_decibel_magnitude.h>

#include <FFT_mels/rfft_output.h>
#include <FFT_mels/rfft_log_magnitude.h>
#include <FFT_mels/rfft_decibel_magnitude.h>

constexpr double
LOGARITHIMIC_ERROR_TOLERANCE = 1e-5;

TEST(ComputeLogarithmsFromComplex, Base) {
    /// Base case; both real and imaginary values are zero
    {
        EXPECT_FLOAT_EQ(
            -379.29779f,
            compute_decibels_from_complex(
                0.0f,
                0.0f,
                0.0f));
    }

    /// Everything else
    {
        constexpr float
        FLOAT_ARRAY[] = {
            1.0f, 15.0f, -20.0f, -100.0f, 1000.0f, 1200.0f, -3000.0f, 0.1f, 0.2f//, 0.9999f, -0.9999f,
        };

        constexpr uint32_t
        FLOAT_ARRAY_LENGTH = sizeof(FLOAT_ARRAY) / sizeof(float);

        for (uint32_t first_iterator = 0; first_iterator < FLOAT_ARRAY_LENGTH; first_iterator++) {
            const float &first_float = FLOAT_ARRAY[first_iterator];

            for (uint32_t second_iterator = first_iterator; second_iterator < FLOAT_ARRAY_LENGTH; second_iterator++) {
                const float &second_float = FLOAT_ARRAY[second_iterator];

                const float magnitude = sqrtf(static_cast<float>(pow(first_float, 2) + pow(second_float, 2)));
                assert(magnitude >= 0.0f);

                const float expected_logarithm_result = log10f(magnitude);
                const float expected_decibel_result = 20.0f * log10f(magnitude);
                assert(expected_decibel_result == 20.0f * expected_logarithm_result);

                /// Check decibel
                const float
                result_decibel_12 = compute_decibels_from_complex(
                    first_float,
                    second_float,
                    0.0f);
                const float
                result_decibel_21 = compute_decibels_from_complex(
                    second_float,
                    first_float,
                    0.0f);
                EXPECT_FLOAT_EQ(
                    result_decibel_12,
                    result_decibel_21);
                EXPECT_NEAR(
                    result_decibel_12,
                    expected_decibel_result,
                    LOGARITHIMIC_ERROR_TOLERANCE);
                EXPECT_NEAR(
                    result_decibel_21,
                    expected_decibel_result,
                    LOGARITHIMIC_ERROR_TOLERANCE);

                /// Check logarithm
                const float
                result_logarithm_12 = compute_log_from_complex(
                    first_float,
                    second_float,
                    0.0f);
                const float
                result_logarithm_21 = compute_log_from_complex(
                    second_float,
                    first_float,
                    0.0f);
                EXPECT_FLOAT_EQ(
                    result_logarithm_12,
                    result_logarithm_21);
                EXPECT_NEAR(
                    result_logarithm_12,
                    expected_logarithm_result,
                    LOGARITHIMIC_ERROR_TOLERANCE);
                EXPECT_NEAR(
                    result_logarithm_21,
                    expected_logarithm_result,
                    LOGARITHIMIC_ERROR_TOLERANCE);

                /// Check relation between functions
                EXPECT_FLOAT_EQ(
                    result_logarithm_12 * 20.0f,
                    result_decibel_12);
            }
        }
    }
}

TEST(ComputeLogarithmsFromComplex, PrecomputeTestAudio2_Audio) {
    /// Check parameters
    {
        assert(
            FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH > 0 &&
            FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH % 2 == 0 &&
            FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH / 2 == FFT_AUDIO_LOG_MAGNITUDE_BUFFER_LENGTH);

        assert(
            FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH > 0 &&
            FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH % 2 == 0 &&
            FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH / 2 == FFT_AUDIO_DECIBEL_MAGNITUDE_BUFFER_LENGTH);
    }

    /// Compute from Test Audio 2
    {
        constexpr uint32_t num_iterations = FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH / 2;
        for (uint32_t iterator = 0; iterator < num_iterations; iterator++) {
            const float
            &real_value = FFT_AUDIO_RFFT_OUTPUT_BUFFER[iterator * 2 + 0];
            const float
            &imaginary_value = FFT_AUDIO_RFFT_OUTPUT_BUFFER[iterator * 2 + 1];

            /// Compare against logarithmic result
            {
                const float
                result_logarithmic = compute_log_from_complex(
                    real_value,
                    imaginary_value,
                    0.0f);
                const float
                &expected_logarithmic_output = FFT_AUDIO_LOG_MAGNITUDE_BUFFER[iterator];

                EXPECT_NEAR(
                    result_logarithmic,
                    expected_logarithmic_output,
                    LOGARITHIMIC_ERROR_TOLERANCE);
            }

            /// Compare against decibel result
            {
                const float
                result_decibel = compute_decibels_from_complex(
                    real_value,
                    imaginary_value,
                    0.0f);
                const float
                &expected_decibel_output = FFT_AUDIO_DECIBEL_MAGNITUDE_BUFFER[iterator];

                EXPECT_NEAR(
                    result_decibel,
                    expected_decibel_output,
                    LOGARITHIMIC_ERROR_TOLERANCE);
            }
        }

    }
}

TEST(ComputeLogarithmsFromComplex, PrecomputeTestAudio2_Mels) {
    /// Check parameters
    {
        assert(
            FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH > 0 &&
            FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH % 2 == 0 &&
            FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH / 2 == FFT_MELS_LOG_MAGNITUDE_BUFFER_LENGTH);

        assert(
            FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH > 0 &&
            FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH % 2 == 0 &&
            FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH / 2 == FFT_MELS_DECIBEL_MAGNITUDE_BUFFER_LENGTH);
    }

    /// Compute from Test Audio 2
    {
        constexpr uint32_t num_iterations = FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH / 2;
        for (uint32_t iterator = 0; iterator < num_iterations; iterator++) {
            const float
            &real_value = FFT_MELS_RFFT_OUTPUT_BUFFER[iterator * 2 + 0];
            const float
            &imaginary_value = FFT_MELS_RFFT_OUTPUT_BUFFER[iterator * 2 + 1];

            /// Compare against logarithmic result
            {
                const float
                result_logarithmic = compute_log_from_complex(
                    real_value,
                    imaginary_value,
                    0.0f);
                const float
                &expected_logarithmic_output = FFT_MELS_LOG_MAGNITUDE_BUFFER[iterator];

                EXPECT_NEAR(
                    result_logarithmic,
                    expected_logarithmic_output,
                    LOGARITHIMIC_ERROR_TOLERANCE);
            }

            /// Compare against decibel result
            {
                const float
                result_decibel = compute_decibels_from_complex(
                    real_value,
                    imaginary_value,
                    0.0f);
                const float
                &expected_decibel_output = FFT_MELS_DECIBEL_MAGNITUDE_BUFFER[iterator];

                EXPECT_NEAR(
                    result_decibel,
                    expected_decibel_output,
                    LOGARITHIMIC_ERROR_TOLERANCE);
            }
        }

    }
}
