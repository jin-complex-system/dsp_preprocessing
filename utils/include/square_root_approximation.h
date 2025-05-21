#ifndef SQUARE_ROOT_APPROXIMATION_H
#define SQUARE_ROOT_APPROXIMATION_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Perform square root approximation
 *
 * Precision loss is acceptable
 *
 * @param target_value float
 * @return sqrt(float)
 */
float
square_root_approximation(
    const float target_value);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SQUARE_ROOT_APPROXIMATION_H
