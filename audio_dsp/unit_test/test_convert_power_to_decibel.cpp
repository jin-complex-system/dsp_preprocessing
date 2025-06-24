#include <gtest/gtest.h>
#include <mel_spectrogram.h>
#include <cmath>

constexpr double
FLOAT_ERROR_TOLERANCE = 1e-5;

TEST(ConvertPowerToDecibel, BaseCase) {
    constexpr float REFERENCE_FLOAT_ARRAY[] = {
        1.0f, 0.5f, 0.25f, 20.0f, 0.0001f, 0.0001913713349495083f};

    for (const auto &reference_float : REFERENCE_FLOAT_ARRAY) {
        /// Check reference_float
        {
            float zero_log = 1.0f;
            const double expected_result =
                10.0 * log10((double)(zero_log)) - 10.0 * log10((double)reference_float);

            convert_power_to_decibel(
                &zero_log,
                1,
                reference_float,
                -1.0f
            );
            EXPECT_NEAR(
                zero_log,
                expected_result,
                FLOAT_ERROR_TOLERANCE
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
                FLOAT_ERROR_TOLERANCE
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

TEST(ConvertPowerToDecibel, TopDecibel) {
    constexpr float LARGE_FLOAT = 1e10f;
    const double EXPECTED_RESULT_WITHOUT_CLIPPING =
        10.0 * log10((double)(LARGE_FLOAT));

    /// Base Case - No top decibel
    {
        float my_float = LARGE_FLOAT;

        convert_power_to_decibel(
            &my_float,
            1,
            1.0f,
            -1.0f
        );
        EXPECT_NEAR(
            my_float,
            EXPECTED_RESULT_WITHOUT_CLIPPING,
            FLOAT_ERROR_TOLERANCE
        );
    }

    /// Base Case - Same as top decibel
    {
        float my_float = LARGE_FLOAT;

        convert_power_to_decibel(
            &my_float,
            1,
            1.0f,
            my_float
        );
        EXPECT_NEAR(
            my_float,
            EXPECTED_RESULT_WITHOUT_CLIPPING,
            FLOAT_ERROR_TOLERANCE
        );
    }

    /// Difference Top decibels but less than expected result
    {
        constexpr float TOP_DECIBELS[] = {
            1.0f,
            20.0f,
            80.0f,
            10.0f * log10f(LARGE_FLOAT) - 0.1f};

        for (const auto &top_decibel : TOP_DECIBELS) {
            assert(top_decibel < EXPECTED_RESULT_WITHOUT_CLIPPING);
            float my_float = LARGE_FLOAT;

            convert_power_to_decibel(
                &my_float,
                1,
                1.0f,
                top_decibel
            );
            EXPECT_NEAR(
                my_float,
                top_decibel,
                FLOAT_ERROR_TOLERANCE
            );
        }
    }
}
