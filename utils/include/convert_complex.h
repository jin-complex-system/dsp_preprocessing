#ifndef CONVERT_COMPLEX_H
#define CONVERT_COMPLEX_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Compute magnitude from a complex number
 *
 * Assumes that precision loss is acceptable, and
 * comparison and fabs() operations are overall cheaper to run
 *
 * @param real_value
 * @param img_value
 * @param scale_factor scale factor applied to the magnitude
 * @return magnitude
 */
float
compute_magnitude_from_complex(
    const float real_value,
    const float img_value,
    const float scale_factor);

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
