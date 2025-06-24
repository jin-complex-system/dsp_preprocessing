#include <gtest/gtest.h>

#include <convert_complex.h>
#include <cassert>
#include <cmath>

static
constexpr
float
FLOAT_ARRAY[] = {
    0.0f, 2000.0f, 0.001f, 1.0f, -1.0f, 2.0f, -2.0f, 4.0f, 16.0f, 0.2f, -0.65f, 99.0f,
    200.0f, 150.0f, 3.1f, -1e-5f, 0.0001913713349495083f, 5*1e-3f,
    3346.006f, 3270.84912109375f,
};

static
constexpr
uint32_t
FLOAT_ARRAY_LENGTH = sizeof(FLOAT_ARRAY) / sizeof(float);

TEST(ComputeMagnitudePowerFromComplex, SingleValue) {
    for (uint32_t first_iterator = 0; first_iterator < FLOAT_ARRAY_LENGTH; first_iterator++) {
        const float &first_float = FLOAT_ARRAY[first_iterator];

        for (uint32_t second_iterator = first_iterator; second_iterator < FLOAT_ARRAY_LENGTH; second_iterator++) {
            const float &second_float = FLOAT_ARRAY[first_iterator];

            const auto expected_power =
                pow(first_float, 2) + pow(second_float, 2);
            const auto expected_magnitude =
                sqrt(expected_power);
            assert(expected_magnitude >= 0.0f);

            float
            input_array[2] = {};
            for (uint32_t scale_iterator = 0; scale_iterator < 1; scale_iterator++) {
                /// first_float represents real
                /// second_float represents imaginary
                {
                    input_array[0] = first_float;
                    input_array[1] = second_float;

                    /// Power
                    {
                        float computed_output = -1.0f;
                        compute_power_from_complex_arrays(
                            input_array,
                            &computed_output,
                            1u);
                        EXPECT_FLOAT_EQ(
                            expected_power,
                            computed_output);
                    }

                    /// Magnitude
                    {
                        float computed_output = -1.0f;
                        compute_magnitude_from_complex_arrays(
                            input_array,
                            &computed_output,
                            1u);
                        EXPECT_FLOAT_EQ(
                            expected_magnitude,
                            computed_output);
                    }
                }

                /// second_float represents real
                /// first_float represents imaginary
                {
                    input_array[0] = second_float;
                    input_array[1] = first_float;

                    /// Power
                    {
                        float computed_output = -1.0f;
                        compute_power_from_complex_arrays(
                            input_array,
                            &computed_output,
                            1u);
                        EXPECT_FLOAT_EQ(
                            expected_power,
                            computed_output);
                    }

                    /// Magnitude
                    {
                        float computed_output = -1.0f;
                        compute_magnitude_from_complex_arrays(
                            input_array,
                            &computed_output,
                            1u);
                        EXPECT_FLOAT_EQ(
                            expected_magnitude,
                            computed_output);
                    }
                }
            }
        }
    }
}

TEST(ComputeMagnitudePowerFromComplex, EntireArray) {
    assert (FLOAT_ARRAY_LENGTH % 2 == 0);
    constexpr auto num_samples =
        (uint32_t)(FLOAT_ARRAY_LENGTH / 2);

    float
    computed_magnitude_array[num_samples];
    float
    computed_power_array[num_samples];

    compute_power_from_complex_arrays(
        FLOAT_ARRAY,
        computed_power_array,
        num_samples);
    compute_magnitude_from_complex_arrays(
        FLOAT_ARRAY,
        computed_magnitude_array,
        num_samples);


    for (uint32_t iterator = 0; iterator < num_samples; iterator++) {
        const float &first_float = FLOAT_ARRAY[iterator * 2 + 0];
        const float &second_float = FLOAT_ARRAY[iterator * 2 + 1];

        const auto expected_power =
            pow(first_float, 2) + pow(second_float, 2);
        const auto expected_magnitude =
            sqrt(expected_power);

        EXPECT_FLOAT_EQ(
            expected_power,
            computed_power_array[iterator]);

        EXPECT_FLOAT_EQ(
            expected_magnitude,
            computed_magnitude_array[iterator]);

    }
}
