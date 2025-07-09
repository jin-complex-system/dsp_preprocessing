#include "scaler_float.h"

#include <stddef.h>
#include <assert.h>
#include <math.h>

void
scale_float_buffer(
    const float* input_buffer,
    float* output_buffer,
    const uint32_t num_elements,
    const float top_value,
    const float bottom_value) {
    /// Check parameters
    {
        assert(input_buffer != NULL);
        assert(output_buffer != NULL);
        assert(num_elements > 0);
        assert(top_value > bottom_value);
    }

    const float value_range_inverted = 1.0f / (top_value - bottom_value);
    assert(!isinf(value_range_inverted) && !isnan(value_range_inverted));

    for (uint32_t iterator = 0; iterator < num_elements; iterator++) {
        if (input_buffer[iterator] > top_value) {
            output_buffer[iterator] = 1.0f;
        }
        else if (input_buffer[iterator] < bottom_value) {
            output_buffer[iterator] = 0.0f;
        }
        else {
            output_buffer[iterator] = (input_buffer[iterator] - bottom_value) * value_range_inverted;
            assert(
                !isinf(output_buffer[iterator]) &&
                !isnan(output_buffer[iterator]));
        }
    }
}

void
scale_float_buffer_quantised(
    const float* input_buffer,
    uint8_t* output_buffer,
    const uint32_t num_elements,
    const float top_value,
    const float bottom_value) {
    /// Check parameters
    {
        assert(input_buffer != NULL);
        assert(output_buffer != NULL);
        assert(num_elements > 0);
        assert(top_value > bottom_value);
    }

    const float value_range_inverted = (255.0f) / (top_value - bottom_value);
    assert(!isinf(value_range_inverted) && !isnan(value_range_inverted));

    for (uint32_t iterator = 0; iterator < num_elements; iterator++) {
        if (input_buffer[iterator] > top_value) {
            output_buffer[iterator] = 255u;
        }
        else if (input_buffer[iterator] < bottom_value) {
            output_buffer[iterator] = 0u;
        }
        else {
            output_buffer[iterator] = (uint8_t)((input_buffer[iterator] - bottom_value) * value_range_inverted);
        }
    }
}

