#ifndef LOG_APPROXIMATION_H
#define LOG_APPROXIMATION_H

#include <stdint.h>

#ifdef __ARM_ARCH
#include "arm_math.h"
#endif //__ARM_ARCH

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Get the minimum supported value for log approximation
 * @return
 */
static inline
const float
get_log_approximation_minimum_supported_value(void) {
    return 1.175494351e-38f;
}

/**
 * Performs log10 approximation
 * @param target_value Positive floats. If 0.0f or less, see _get_log10_approximation_minimum_value()
 * @return
 */
float
log10_approximation(
    const float target_value);

/**
 * Used internally for log10_approximation() to handle input parameter 0.0f or less
 * @return log10_approximation()'s minimum value
 */
static inline
const float
_get_log10_approximation_minimum_value(void) {
    return -37.929779052734375f;
}

/**
 * Performs log2 approximation
 * @param target_value Positive floats. If 0.0f or less, see _get_log2_approximation_minimum_value()
 * @return
 */
float
log2_approximation(
    const float target_value);

/**
 * Used internally for log2_approximation() to handle input parameter 0.0f or less
 * @return log2_approximation()'s minimum value
 */
static inline
const float
_get_log2_approximation_minimum_value(void) {
    return -126.0f;
}

/**
 * Performs log e approximation
 * @param target_value Positive floats. If 0.0f or less, see _get_loge_approximation_minimum_value()
 * @return
 */
float
loge_approximation(
    const float target_value);

/**
 * Used internally for loge_approximation() to handle input parameter 0.0f or less.
 *
 * Not used for vector mathematics
 *
 * @return loge_approximation()'s minimum value
 */
static inline
const float
_get_loge_approximation_minimum_value(void) {
    return -87.3327190953f;
}

/**
 * Performs vector log e approximation. Values less than 0.0f may not be supported
 *
 * If hardware-specific algorithms is not available, defaults to loge_approximation()
 *
 * @param pSource
 * @param pDestination if pDestination == pSource, performs calculation in-place
 * @param numElements number of elements
 */
void
v_loge_approximation(
    const float* pSource,
    float* pDestination,
    const uint32_t numElements);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //LOG_APPROXIMATION_H
