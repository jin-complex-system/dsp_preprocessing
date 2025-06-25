#include <gtest/gtest.h>

#include <log_approximation.h>
#include <cassert>
#include <cmath>
#include <cstring>

static constexpr double
LOG2_FLOAT_ERROR_TOLERANCE = 1e-29;

static constexpr double
LOG10_FLOAT_ERROR_TOLERANCE = 4.1 * 1e-7;

static constexpr double
LOGE_FLOAT_ERROR_TOLERANCE = 3.827 * 1e-3;

static const float
MINIMUM_FLOAT_VALUE = get_log_approximation_minimum_supported_value();

TEST(LogApproximation, SingleBase) {
    /// Check handling of 0.0f, 1.0f and negative values
    {
        constexpr float zero_float = 0.0f;
        constexpr float one_float = 1.0f;
        constexpr float negative_float = -1.0f * one_float;

        /// Check value of 0.0f and negative values
        {
            EXPECT_FLOAT_EQ(log10_approximation(zero_float), _get_log10_approximation_minimum_value());
            EXPECT_FLOAT_EQ(log2_approximation(zero_float), _get_log2_approximation_minimum_value());
            EXPECT_FLOAT_EQ(loge_approximation(zero_float), _get_loge_approximation_minimum_value());

            EXPECT_FLOAT_EQ(log10_approximation(negative_float), _get_log10_approximation_minimum_value());
            EXPECT_FLOAT_EQ(log2_approximation(negative_float), _get_log2_approximation_minimum_value());
            EXPECT_FLOAT_EQ(loge_approximation(negative_float), _get_loge_approximation_minimum_value());
        }

        /// Check value of 1.0f
        {
            EXPECT_FLOAT_EQ(log10_approximation(one_float), zero_float);
            EXPECT_FLOAT_EQ(log2_approximation(one_float), zero_float);
            EXPECT_FLOAT_EQ(loge_approximation(one_float), zero_float);
        }

        /// Checking the minimum value
        {
            EXPECT_NEAR(
                log2_approximation(MINIMUM_FLOAT_VALUE),
                log2((double)MINIMUM_FLOAT_VALUE),
                LOG2_FLOAT_ERROR_TOLERANCE);
            EXPECT_NEAR(
                log10_approximation(MINIMUM_FLOAT_VALUE),
                log10((double)MINIMUM_FLOAT_VALUE),
                LOG10_FLOAT_ERROR_TOLERANCE);
            EXPECT_NEAR(
                loge_approximation(MINIMUM_FLOAT_VALUE),
                log((double)MINIMUM_FLOAT_VALUE),
                LOGE_FLOAT_ERROR_TOLERANCE);
        }
    }
}

TEST(LogApproximation, VectorBase) {
    /// Check handling of 0.0f, 1.0f and negative values
    {
        constexpr float zero_float = 0.0f;
        constexpr float one_float = 1.0f;
        constexpr float negative_float = -1.0f * one_float;

        /// Check value of 0.0f
        {
            // May not be valid this for vector mathematics
            {
                float output_float = negative_float;
                v_loge_approximation(
                        &zero_float,
                        &output_float,
                        1);
                EXPECT_NE(output_float, negative_float);
                EXPECT_FLOAT_EQ(output_float, _get_loge_approximation_minimum_value());
            }
        }

        /// Check value of 1.0f
        {
            // May not be valid this for vector mathematics
            {
                float output_float = negative_float;
                v_loge_approximation(
                        &one_float,
                        &output_float,
                        1);
                EXPECT_NE(output_float, negative_float);
                EXPECT_FLOAT_EQ(output_float, zero_float);
            }
        }

        /// Check handling of negative values
        {
            // May not be valid this for vector mathematics
            {
                float output_float = negative_float;
                v_loge_approximation(
                        &negative_float,
                        &output_float,
                        1);
                EXPECT_NE(output_float, negative_float);
                EXPECT_FLOAT_EQ(output_float, _get_loge_approximation_minimum_value());
            }
        }

        /// Checking the minimum value
        {
            {
                float output_float = negative_float;
                v_loge_approximation(
                        &MINIMUM_FLOAT_VALUE,
                        &output_float,
                        1);
                EXPECT_NE(output_float, negative_float);
                EXPECT_NEAR(
                    output_float,
                    log((double)MINIMUM_FLOAT_VALUE),
                    LOGE_FLOAT_ERROR_TOLERANCE);
            }
        }
    }
}

TEST(LogApproximation, LogE)
{
    constexpr float
    INPUT_FLOAT_ARRAY[] = {
        1.0f, 200.0f, 500.0f, 0.2f, 1e-4f};
    constexpr auto
    NUM_ELEMENTS = (uint32_t)sizeof(INPUT_FLOAT_ARRAY) / sizeof(INPUT_FLOAT_ARRAY[0]);


    float v_loge_output_array[NUM_ELEMENTS] = {};  // Only used to store output
    float v_loge_compute_in_place_array[NUM_ELEMENTS] = {};  // Buffer to compute in-place

    /// Compute results for values above 0.0f
    /// v_loge_approximation() may handle values <= 0.0f differently than loge_approximation()
    for (uint32_t num_element = 1; num_element < NUM_ELEMENTS; num_element++) {
        assert(INPUT_FLOAT_ARRAY[num_element - 1] > _get_loge_approximation_minimum_value());

        /// Prepare buffers
        {
            memset(v_loge_output_array, 0, sizeof(v_loge_output_array));
            memset(v_loge_compute_in_place_array, 0, sizeof(v_loge_output_array));

            const auto MEMCPY_RESULT =
            memcpy(v_loge_compute_in_place_array, INPUT_FLOAT_ARRAY, sizeof(INPUT_FLOAT_ARRAY));
            assert(MEMCPY_RESULT == &v_loge_compute_in_place_array[0]);
            const auto MEMCMP_RESULT =
            memcmp(v_loge_compute_in_place_array, INPUT_FLOAT_ARRAY, sizeof(v_loge_compute_in_place_array));
            assert(MEMCMP_RESULT == 0);
        }

        /// Compute
        v_loge_approximation(
            INPUT_FLOAT_ARRAY,
            v_loge_output_array,
            num_element);
        v_loge_approximation(
            v_loge_compute_in_place_array,
            v_loge_compute_in_place_array,
            num_element);

        EXPECT_EQ(0, memcmp(v_loge_compute_in_place_array, v_loge_output_array, num_element * sizeof(float)));

        /// Check values
        for (uint32_t iterator = 0; iterator < num_element; iterator++) {
            const auto &input_float = INPUT_FLOAT_ARRAY[iterator];
            const auto &computed_vector_result = v_loge_output_array[iterator];
            const auto &computed_single_result = loge_approximation(input_float);
            const auto &expected_result = log((double)input_float);

            EXPECT_NE(input_float, computed_vector_result);
            EXPECT_FLOAT_EQ(computed_vector_result, computed_single_result);
            EXPECT_NEAR(expected_result, computed_vector_result, LOGE_FLOAT_ERROR_TOLERANCE);
        }
    }
}

