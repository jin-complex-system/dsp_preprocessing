#include <gtest/gtest.h>

#include <cmath>
#include <square_root_approximation.h>

static constexpr
double
MINIMUM_FLOAT_VALUE = 1.175494351e-38;

constexpr
double
SQUARE_ROOT_APPROXIMATION_ERROR_TOLERANCE = 1e-4 + MINIMUM_FLOAT_VALUE;

TEST(SquareRootApproximation, Base) {
    constexpr float input_values[] = {
        0.0f, 1.0f, 2.0f, 4.0f, 3200.0f, 10.0f, 2.0f, 0.5f, 0.25f, 1e-5f, 1e-6f,
        powf(44.72136f, 2), powf(3346.006f, 2), powf(3270.84912109375f, 2)
    };

    for (const auto input_value : input_values) {
        const float
        square_root_approximation_result = square_root_approximation(input_value);

        EXPECT_NEAR(
            sqrt(input_value + MINIMUM_FLOAT_VALUE),
            square_root_approximation_result,
            SQUARE_ROOT_APPROXIMATION_ERROR_TOLERANCE
        );
    }
}