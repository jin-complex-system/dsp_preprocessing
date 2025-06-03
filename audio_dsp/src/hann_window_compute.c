#include "hann_window_compute.h"
#include <math.h>
#include <assert.h>

const double
MY_2_PI = 6.28318530717958623199592693708837032318115234375;

// const double
// MY_2_PI = 2 * M_PI;

void
hann_window_compute(
    float* hann_window_buffer,
    const uint32_t hann_window_buffer_length,
    const float scaling_factor) {
    /// Check parameters
    assert(hann_window_buffer_length > 1);
    assert(scaling_factor > 0.0f);

    for (uint32_t iterator = 0; iterator < hann_window_buffer_length; iterator++) {

        /// Match librosa's python implementation,
        /// which favours scipy.signal.get_window() implementation rather than scipy.signal.windows.hann()
        double hann_value_double = 0.5 - 0.5 * cos((MY_2_PI * iterator) / hann_window_buffer_length);
        // double hann_value_double = 0.5 - 0.5 * cos((MY_2_PI * iterator) / (hann_window_buffer_length - 1));
        hann_value_double = hann_value_double * scaling_factor;

        float hann_value_float = 0.0f;

        /// If very small values, set to 0
        if (!isinf(hann_value_double) && !isnan(hann_value_double)) {
            hann_value_float = (float)hann_value_double;
        }
        /// If very small values, set to 0
        if (isinf(hann_value_float) || isnan(hann_value_float)) {
            hann_value_float = 0.0f;
        }

        assert(hann_value_float >= 0.0f);
        hann_window_buffer[iterator] = hann_value_float;
    }
}
