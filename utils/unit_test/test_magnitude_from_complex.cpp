#include <gtest/gtest.h>

#include <convert_complex.h>
#include <cassert>
#include <cmath>

static
constexpr
float
FLOAT_ARRAY[] = {
    0.0f, 1.0f, -1.0f, 2.0f, -2.0f, 4.0f, 16.0f, 0.2f, -0.65f, 99.0f,
    200.0f, 150.0f, 3.1f, -1e-5f, 0.0001913713349495083f, 5*1e-3f,
};

static
constexpr
uint32_t
FLOAT_ARRAY_LENGTH = sizeof(FLOAT_ARRAY) / sizeof(float);

TEST(ComputeMagnitudeFromComplex, SingleValue) {
    for (uint32_t first_iterator = 0; first_iterator < FLOAT_ARRAY_LENGTH; first_iterator++) {
        const float &first_float = FLOAT_ARRAY[first_iterator];

        for (uint32_t second_iterator = first_iterator; second_iterator < FLOAT_ARRAY_LENGTH; second_iterator++) {

            const float &second_float = FLOAT_ARRAY[first_iterator];

            const float expected_magnitude = sqrtf(static_cast<float>(pow(first_float, 2) + pow(second_float, 2)));
            assert(expected_magnitude >= 0.0f);

            float
            input_array[2] = {};
            float computed_output;

            for (uint32_t scale_iterator = 0; scale_iterator < 1; scale_iterator++) {
                const float scale_factor = input_array[scale_iterator];

                /// first_float represents real
                /// second_float represents imaginary
                {
                    input_array[0] = first_float;
                    input_array[1] = second_float;

                    compute_magnitude_from_complex_arrays(
                        input_array,
                        &computed_output,
                        1u,
                        1.0f);

                    EXPECT_FLOAT_EQ(
                        expected_magnitude,
                        computed_output);

                    if (scale_factor != 0.0f && scale_factor != 1.0f) {
                        compute_magnitude_from_complex_arrays(
                            input_array,
                            &computed_output,
                            1u,
                            scale_factor);
                        EXPECT_FLOAT_EQ(
                            expected_magnitude * scale_factor,
                            computed_output);
                    }
                }

                /// second_float represents real
                /// first_float represents imaginary
                {
                    input_array[0] = second_float;
                    input_array[1] = first_float;

                    compute_magnitude_from_complex_arrays(
                        input_array,
                        &computed_output,
                        1u,
                        1.0f);

                    EXPECT_FLOAT_EQ(
                        expected_magnitude,
                        computed_output);

                    if (scale_factor != 0.0f && scale_factor != 1.0f) {
                        compute_magnitude_from_complex_arrays(
                            input_array,
                            &computed_output,
                            1u,
                            scale_factor);
                        EXPECT_FLOAT_EQ(
                            expected_magnitude * scale_factor,
                            computed_output);
                    }
                }
            }
        }
    }
}

TEST(ComputeMagnitudeFromComplex, EntireArray) {
    assert (FLOAT_ARRAY_LENGTH % 2 == 0);
    constexpr auto num_samples =
        (uint32_t)(FLOAT_ARRAY_LENGTH / 2);

    float
    computed_output_array[num_samples];

    compute_magnitude_from_complex_arrays(
        FLOAT_ARRAY,
        computed_output_array,
        num_samples,
        1.0f);

    for (uint32_t iterator = 0; iterator < num_samples; iterator++) {
        const float &first_float = FLOAT_ARRAY[iterator * 2 + 0];
        const float &second_float = FLOAT_ARRAY[iterator * 2 + 1];

        const float expected_magnitude =
            sqrtf(static_cast<float>(pow(first_float, 2) + pow(second_float, 2)));

        EXPECT_FLOAT_EQ(
            expected_magnitude,
            computed_output_array[iterator]);
    }
}
