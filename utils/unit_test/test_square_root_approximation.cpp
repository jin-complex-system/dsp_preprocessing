#include <gtest/gtest.h>

#include <cmath>
#include <square_root_approximation.h>

constexpr double
SQUARE_ROOT_APPROXIMATION_ERROR_TOLERANCE = 1e-4;

TEST(SquareRootApproximation, Base) {
    constexpr float input_values[] = {
        10.0f, 2.0f, 0.5f, 0.25f,
    };

    for (const auto input_value : input_values)
    {
        const float
        square_root_approximation_result = square_root_approximation(input_value);

        EXPECT_NEAR(
            sqrt(input_value),
            square_root_approximation_result,
            SQUARE_ROOT_APPROXIMATION_ERROR_TOLERANCE
        );
    }
}