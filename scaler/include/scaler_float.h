#ifndef SCALER_FLOAT_H
#define SCALER_FLOAT_H

#include <stdint.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Scale input_buffer to [0.0, 1.0] as float. Clips between top_value and bottom_value
 * @param input_buffer
 * @param output_buffer 
 * @param num_elements 
 * @param top_value 
 * @param bottom_value 
 */
void
scale_float_buffer(
    const float* input_buffer,
    float* output_buffer,
    const uint32_t num_elements,
    const float top_value,
    const float bottom_value);

/**
 * Scale input_buffer to [0, 255] as uint8. Clips between top_value and bottom_value
 * @param input_buffer 
 * @param output_buffer 
 * @param num_elements 
 * @param top_value 
 * @param bottom_value 
 */
void
scale_float_buffer_quantised(
    const float* input_buffer,
    uint8_t* output_buffer,
    const uint32_t num_elements,
    const float top_value,
    const float bottom_value);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //SCALER_FLOAT_H
