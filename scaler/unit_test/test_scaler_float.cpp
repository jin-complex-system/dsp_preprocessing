#include <gtest/gtest.h>

#include <scaler_float.h>
#include <cstdint>

TEST(ScalerFloat, Base) {
    constexpr float INPUT_ARRAY[] = {
        -500.0f, 500.0f, // min and max of this array
        0.0f, 1.0f, 2.0f, 0.5f, 3.0f, 5.0f, 1e-5f,
    };
    constexpr uint32_t NUM_ELEMENTS = sizeof(INPUT_ARRAY) / sizeof(INPUT_ARRAY[0]);
    constexpr auto MIN_INPUT_ARRAY = (double)INPUT_ARRAY[0];
    constexpr auto MAX_INPUT_ARRAY = (double)INPUT_ARRAY[1];
    constexpr auto INPUT_ARRAY_RANGE = MAX_INPUT_ARRAY - MIN_INPUT_ARRAY;

    assert(MAX_INPUT_ARRAY > MIN_INPUT_ARRAY);

    /// Use bounds, depending on max and min of INPUT_ARRAY
    {
        float output_float_buffer[NUM_ELEMENTS];
        scale_float_buffer(
            INPUT_ARRAY,
            output_float_buffer,
            NUM_ELEMENTS,
            MAX_INPUT_ARRAY,
            MIN_INPUT_ARRAY
        );

        uint8_t output_uint8_buffer[NUM_ELEMENTS];
        scale_float_buffer_quantised(
            INPUT_ARRAY,
            output_uint8_buffer,
            NUM_ELEMENTS,
            MAX_INPUT_ARRAY,
            MIN_INPUT_ARRAY
        );

        for (uint32_t iterator = 0; iterator < NUM_ELEMENTS; iterator++) {
            const float &input = INPUT_ARRAY[iterator];
            double float_output;

            if (input > MAX_INPUT_ARRAY) {
                float_output = 1.0;
            }
            else if (input < MIN_INPUT_ARRAY) {
                float_output = 0.0;
            }
            else {
                float_output = (input - MAX_INPUT_ARRAY) / INPUT_ARRAY_RANGE;
            }
            const auto uint8_output = (uint8_t)(float_output * 255.0);

            EXPECT_NEAR(
                float_output,
                output_float_buffer[iterator],
                1e-7);
            EXPECT_EQ(
                uint8_output,
                output_uint8_buffer[iterator]);
        }
    }
}
