#ifndef CONVERT_COMPLEX_H
#define CONVERT_COMPLEX_H

#include <stdint.h>

#ifdef __ARM_ARCH
#include "arm_math.h"
#endif //__ARM_ARCH

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Compute magnitude from an array of complex numbers
 *
 * @param complex_input_vector float input vector containing complex floats
 * @param output_array real magnitude
 * @param num_samples input and output array length is 2*num_samples and num_samples respectively
 */
void
compute_magnitude_from_complex_arrays(
    const float* complex_input_vector,
    float* output_array,
    const uint32_t num_samples);

/**
 * Compute power (magnitude ^ 2) from an array of complex numbers
 * @param complex_input_vector float input vector containing complex floats
 * @param output_array real power
 * @param num_samples input and output array length is 2*num_samples and num_samples respectively
 */
void
compute_power_from_complex_arrays(
    const float* complex_input_vector,
    float* output_array,
    const uint32_t num_samples);

/**
 * Compute logarithmic from complex number
 *
 * Assumes that precision loss is acceptable, and
 * comparison operations are overall cheaper to run.
 *
 * Formula is log2(real_value ^ 2 + img_value ^ 2) / (2 * log2(10)) + log_scale_factor
 *
 * @param real_value
 * @param img_value
 * @param log_scale_factor scale factor applied to the magnitude
 * @return logarithimic magnitude
 */
float
compute_log_from_complex(
    const float real_value,
    const float img_value,
    const float log_scale_factor);

/**
 * Compute decibels from complex number
 *
 * Assumes that precision loss is acceptable, and
 * comparison operations are overall cheaper to run.
 *
 * Formula is (10 / log2(10)) * log2(real_value ^ 2 + img_value ^ 2) + log_scale_factor
 *
 * @param real_value
 * @param img_value
 * @param log_scale_factor scale factor applied to the magnitude
 * @return decibel magnitude
 */
float
compute_decibels_from_complex(
    const float real_value,
    const float img_value,
    const float log_scale_factor);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //CONVERT_COMPLEX_H
