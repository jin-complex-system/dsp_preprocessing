#ifndef LOG_APPROXIMATION_H
#define LOG_APPROXIMATION_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Performs log10 approximation
 * @param target_value Positive floats. If 0.0f or less, returns -37.929779052734375f immediately
 * @return
 */
float
log10_approximation(
    const float target_value);

/**
 * Performs log2 approximation
 * @param target_value Positive floats. If 0.0f or less, returns -126.0f immediately
 * @return
 */
float
log2_approximation(
    const float target_value);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //LOG_APPROXIMATION_H
