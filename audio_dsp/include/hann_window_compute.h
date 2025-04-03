#ifndef HANN_WINDOW_COMPUTE_H
#define HANN_WINDOW_COMPUTE_H

#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
//DOM-IGNORE-END

/**
 * Compute hann window manually. Recommended to only be called once
 *
 * Formula is (0.5 - 0.5 * cos( 2 * PI * iterator / (hann_window_buffer_length - 1))) * scaling_factor
 *
 * @param hann_window_buffer buffer to be write
 * @param hann_window_buffer_length length greater than 1
 * @param scaling_factor positive, non-zero value to scale individual elements
 */
void
hann_window_compute(
    float* hann_window_buffer,
    const uint32_t hann_window_buffer_length,
    const float scaling_factor);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
};
#endif
//DOM-IGNORE-END

#endif //HANN_WINDOW_COMPUTE_H
