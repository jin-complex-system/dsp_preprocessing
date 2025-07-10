#include <gtest/gtest.h>
#include <power_to_decibel.h>
#include <cmath>
#include <cstring>
#include <scaler_float.h>

static const
float
MAX_DECIBEL = _power_to_decibel_get_max_decibel();

static const
float
MIN_DECIBEL = _power_to_decibel_get_min_decibel();

constexpr float
MAX_VALUE_INPUT = 9999.999f;

TEST(PowerToDecibelScale, SingleValues) {
    constexpr float REFERENCE_POWER_ARRAY[] = {
        MAX_VALUE_INPUT,
        MAX_VALUE_INPUT - 100.0f,
        MAX_VALUE_INPUT + 100.0f, // Results in computed_result being 254 instead of 255, which is expected
        MAX_VALUE_INPUT / 2,
    };

    for (const auto &reference_power : REFERENCE_POWER_ARRAY) {
        /// Input is MAX_VALUE_INPUT
        {
            float my_input_float = MAX_VALUE_INPUT;
            uint8_t computed_result = 0;

            convert_power_to_decibel_and_scale(
                &my_input_float,
                &computed_result,
                1,
                reference_power
            );

            /// Test if we can use the same buffer
            {
                float reusable_buffer[] = {MAX_VALUE_INPUT};
                auto output_buffer = (uint8_t*)(&reusable_buffer[0]);

                convert_power_to_decibel_and_scale(
                    reusable_buffer,
                    output_buffer,
                    1,
                    reference_power
                );
                EXPECT_EQ(output_buffer[0], computed_result);
            }

            /// Manually compute and compare
            {
                constexpr double input_power = MAX_VALUE_INPUT;
                constexpr double decibel = log(input_power) * 10 / log(10);
                const double reference_decibel = log((double)reference_power) * 10 / log(10);
                const double scaled_decibel = decibel - reference_decibel;
                const double bounded_decibel = (scaled_decibel - MIN_DECIBEL)/(MAX_DECIBEL - MIN_DECIBEL);

                if (bounded_decibel >= 1.0)
                {
                    constexpr uint8_t expected_decibel_uint8 = UINT8_MAX;

                    /// Some minor differences spotted, usually by one integer
                    {
                        EXPECT_NEAR((double)computed_result, (double)expected_decibel_uint8, 1.0);
                    }
                }
                else if (bounded_decibel <= 0.0)
                {
                    constexpr uint8_t expected_decibel_uint8 = 0;
                    EXPECT_EQ(computed_result, expected_decibel_uint8);
                }
                else {
                    const auto expected_decibel_uint8 = (uint8_t)(bounded_decibel * UINT8_MAX);

                    EXPECT_EQ(computed_result, expected_decibel_uint8);
                }
            }

            /// Compare against convert_power_to_decibel() and scaler
            {
                float scale_input = MAX_VALUE_INPUT;
                uint8_t scale_result = 0;
                convert_power_to_decibel(
                    &scale_input,
                    1,
                    reference_power,
                    MAX_DECIBEL
                );
                scale_float_buffer_quantised(
                    &scale_input,
                    &scale_result,
                    1,
                    MAX_DECIBEL,
                    MIN_DECIBEL
                );

                EXPECT_EQ(computed_result, scale_result);
            }
        }
    }
}

TEST(PowerToDecibelScale, Array) {
    const
    float
    INPUT_FLOAT_ARRAY[] = {
        MAX_VALUE_INPUT,
        -5.0f, -25.0f, -1e-8f,
        1.0f, 0.5f, 0.25f, 20.0f, 0.0001f, 0.0001913713349495083f, 500.0f, 200.0f,
        _get_minimum_power(),
    };

    constexpr
    uint16_t
    NUM_ELEMENTS = sizeof(INPUT_FLOAT_ARRAY)/sizeof(INPUT_FLOAT_ARRAY[0]);

    /// Iterate through INPUT_FLOAT_ARRAY
    for (const auto &reference_power : INPUT_FLOAT_ARRAY) {
        const float &abs_reference_power = abs(reference_power);

        /// Prepare buffers
        uint8_t target_buffer[NUM_ELEMENTS] = {};
        float computed_input[NUM_ELEMENTS] = {};
        memcpy(computed_input, INPUT_FLOAT_ARRAY, sizeof(computed_input));

        uint8_t scale_output[NUM_ELEMENTS] = {};
        float scale_input[NUM_ELEMENTS] = {};
        memcpy(scale_input, INPUT_FLOAT_ARRAY, sizeof(computed_input));
        assert(0 == memcmp(scale_input, INPUT_FLOAT_ARRAY, NUM_ELEMENTS * sizeof(float)));

        float reusable_buffer[NUM_ELEMENTS];
        memcpy(reusable_buffer, INPUT_FLOAT_ARRAY, sizeof(reusable_buffer));
        assert(0 == memcmp(reusable_buffer, INPUT_FLOAT_ARRAY, NUM_ELEMENTS * sizeof(float)));

        /// Compute results, with different input and output buffers
        convert_power_to_decibel_and_scale(
            computed_input,
            target_buffer,
            NUM_ELEMENTS,
            abs_reference_power);

        /// Compute results using the same buffer
        convert_power_to_decibel_and_scale(
            reusable_buffer,
            (uint8_t*)reusable_buffer,
            NUM_ELEMENTS,
            abs_reference_power);

        /// Compare against convert_power_to_decibel() and scaler
        memcpy(computed_input, INPUT_FLOAT_ARRAY, sizeof(computed_input));
        convert_power_to_decibel(
            scale_input,
            NUM_ELEMENTS,
            abs_reference_power,
            MAX_DECIBEL
        );
        scale_float_buffer_quantised(
            scale_input,
            scale_output,
            NUM_ELEMENTS,
            MAX_DECIBEL,
            MIN_DECIBEL
        );

        /// Compare results
        for (uint32_t iterator = 0; iterator < NUM_ELEMENTS; iterator++) {
            EXPECT_EQ(target_buffer[iterator], scale_output[iterator]);

            uint8_t* p_reusable_buffer_as_uint8 = (uint8_t*)(reusable_buffer);
            assert(p_reusable_buffer_as_uint8 != nullptr);
            EXPECT_EQ(p_reusable_buffer_as_uint8[iterator], scale_output[iterator]);
        }
    }
}
