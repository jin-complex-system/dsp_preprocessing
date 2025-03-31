#include <gtest/gtest.h>

#include <log_approximation.h>
#include <cassert>
#include <cmath>

constexpr double
LOG2_FLOAT_ERROR_TOLERANCE = 1e-12;

constexpr double
LOG10_FLOAT_ERROR_TOLERANCE = 5 * 1e-7;

TEST(LogApproximation, Base) {
    /// Check handling of 0.0f
    {
        EXPECT_FLOAT_EQ(log10_approximation(0.0f), -37.929779052734375f);
        EXPECT_FLOAT_EQ(log2_approximation(0.0f), -126.0f);
    }

    /// Check the value of 1.0f
    {
        EXPECT_FLOAT_EQ(log2_approximation(1.0f), 0.0f);
        EXPECT_FLOAT_EQ(log10_approximation(1.0f), 0.0f);
    }

    /// Checking the minimum value
    {
        constexpr
        float
        MINIMUM_FLOAT_VALUE = 1.175494351e-38f;

        EXPECT_NEAR(
            log2_approximation(MINIMUM_FLOAT_VALUE),
            log2((double)MINIMUM_FLOAT_VALUE),
            LOG2_FLOAT_ERROR_TOLERANCE);

        EXPECT_NEAR(
            log10_approximation(MINIMUM_FLOAT_VALUE),
            log10((double)MINIMUM_FLOAT_VALUE),
            LOG10_FLOAT_ERROR_TOLERANCE);
    }

}