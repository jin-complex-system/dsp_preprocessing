#include <gtest/gtest.h>

#include <convert_complex.h>
#include <cassert>
#include <cmath>

TEST(ComputeMagnitudeFromComplex, Base) {
    constexpr float
    FLOAT_ARRAY[] = {
        0.0f, 1.0f, -1.0f, 2.0f, -2.0f,
    };
    constexpr uint32_t
    FLOAT_ARRAY_LENGTH = sizeof(FLOAT_ARRAY) / sizeof(float);

    for (uint32_t first_iterator = 0; first_iterator < FLOAT_ARRAY_LENGTH; first_iterator++) {
        const float first_float = FLOAT_ARRAY[first_iterator];

        for (uint32_t second_iterator = first_iterator; second_iterator < FLOAT_ARRAY_LENGTH; second_iterator++) {
            const float second_float = FLOAT_ARRAY[first_iterator];

            const float expected_magnitude = sqrtf(static_cast<float>(pow(first_float, 2) + pow(second_float, 2)));
            assert(expected_magnitude >= 0.0f);

            EXPECT_FLOAT_EQ(
                expected_magnitude,
                compute_magnitude_from_complex(
                    first_float,
                    second_float,
                    1.0f));
            EXPECT_FLOAT_EQ(
                expected_magnitude,
                compute_magnitude_from_complex(
                    second_float,
                    first_float,
                    1.0f));
        }
    }
}