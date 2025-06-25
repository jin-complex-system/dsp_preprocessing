#include <gtest/gtest.h>
#include <power_to_decibel.h>
#include <cmath>

constexpr double
FLOAT_SMALL_VALUES_ERROR_TOLERANCE = 2.498406566076028e-06;

constexpr double
FLOAT_LARGE_VALUES_ERROR_TOLERANCE = 1.5345825488566334e-05;

#define MINIUMUM_SUPPORTED_POWER        _get_minimum_power()
#define MINIMUM_SUPPORTED_DECIBEL       _get_minimum_decibel()


TEST(PowerToDecibel, ConvertPowerToDecibel) {
    constexpr float REFERENCE_FLOAT_ARRAY[] = {
        1.0f, 0.5f, 0.25f, 20.0f, 0.0001f, 0.0001913713349495083f};

    /// Base
    {
        /// Minimum supported value
        {
            float float_buffer = MINIUMUM_SUPPORTED_POWER;
            constexpr float reference_float = 1.0f;

            const double expected_nan_result =
                10.0 * log10((double)(float_buffer)) - 10.0 * log10((double)reference_float);

            convert_power_to_decibel(
                &float_buffer,
                1,
                reference_float,
                -1.0f
            );
            EXPECT_NE(float_buffer, expected_nan_result);
            EXPECT_LE(float_buffer,-80.0); // Supports below -80.0 dB
            EXPECT_NEAR(
                float_buffer,
                MINIMUM_SUPPORTED_DECIBEL,
                FLOAT_SMALL_VALUES_ERROR_TOLERANCE
            );
        }
    }


    for (const auto &reference_float : REFERENCE_FLOAT_ARRAY) {
        /// Check reference_float
        {
            float zero_log_float_input = 1.0f;
            const double expected_result =
                10.0 * log10((double)(zero_log_float_input)) - 10.0 * log10((double)reference_float);

            convert_power_to_decibel(
                &zero_log_float_input,
                1,
                reference_float,
                -1.0f
            );
            EXPECT_NEAR(
                zero_log_float_input,
                expected_result,
                FLOAT_SMALL_VALUES_ERROR_TOLERANCE
            );
        }

        /// Positive values
        {
            float my_positive_float = reference_float;
            const double expected_result =
                10.0 * log10((double)(my_positive_float)) - 10.0 * log10((double)reference_float);

            convert_power_to_decibel(
                &my_positive_float,
                1,
                reference_float,
                -1.0f
            );

            EXPECT_NEAR(
                my_positive_float,
                expected_result,
                FLOAT_SMALL_VALUES_ERROR_TOLERANCE
            );
        }

        /// Handle negative values by having it less than zero
        {
            float my_negative_float = reference_float * -1.0f;
            assert(my_negative_float < 0.0f);

            convert_power_to_decibel(
                &my_negative_float,
                1,
                1.0f,
                -1.0f
            );
            EXPECT_LE(my_negative_float, 0.0f);
        }
    }
}

TEST(PowerToDecibel, ConvertPowerToDecibelTopDecibel) {
    constexpr float LARGE_FLOAT_INPUT = 1e20;
    constexpr float REFERENCE_FLOAT = 1.0f;
    constexpr double EXPECTED_RESULT_WITHOUT_CLIPPING =
        10.0 * log10((double)(LARGE_FLOAT_INPUT) - 10.0 * log10((double)REFERENCE_FLOAT));

    /// Base Case - No top decibel so no clipping
    {
        float my_float = LARGE_FLOAT_INPUT;

        convert_power_to_decibel(
            &my_float,
            1,
            REFERENCE_FLOAT,
            -1.0f
        );
        EXPECT_NEAR(
            my_float,
            EXPECTED_RESULT_WITHOUT_CLIPPING,
            FLOAT_LARGE_VALUES_ERROR_TOLERANCE
        );
    }

    /// Base Case - Same as top decibel so no clipping
    {
        float my_float = LARGE_FLOAT_INPUT;

        convert_power_to_decibel(
            &my_float,
            1,
            REFERENCE_FLOAT,
            my_float
        );
        EXPECT_NEAR(
            my_float,
            EXPECTED_RESULT_WITHOUT_CLIPPING,
            FLOAT_LARGE_VALUES_ERROR_TOLERANCE
        );
    }

    /// Clipping to top decibel
    {
        constexpr float TOP_DECIBELS[] = {
            1.0f,
            20.0f,
            80.0f,
        };

        for (const auto &top_decibel : TOP_DECIBELS) {
            assert(top_decibel < EXPECTED_RESULT_WITHOUT_CLIPPING);

            float my_float = LARGE_FLOAT_INPUT;
            convert_power_to_decibel(
                &my_float,
                1,
                REFERENCE_FLOAT,
                top_decibel
            );
            EXPECT_FLOAT_EQ(my_float, top_decibel);
        }
    }
}
