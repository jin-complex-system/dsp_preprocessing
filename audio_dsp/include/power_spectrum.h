#ifndef AUDIO_DSP_H
#define AUDIO_DSP_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef int16_t audio_data_type;

/**
 * Initialise power spectrum function
 * @param n_fft
 */
void
initialise_power_spectrum(
    const uint16_t n_fft);

/**
 * Deinitialise power spectrum function
 */
void
deinit_power_spectrum(void);

/**
 * Export the power spectrum from input_samples_array
 * @param input_samples_array input samples array; will be converted from audio_data_type to float
 * @param input_samples_array_length same length as n_fft
 * @param output_buffer output buffer containing power spectrum; can be used as intermediate step buffer
 * @param output_buffer_length length of (n_fft/2 + 1) is scratch_buffer_length is non-zero; (n_fft * 2) otherwise
 * @param scratch_buffer scratch buffer is scratch_buffer_length is non-zero
 * @param scratch_buffer_length if zero, do not use scratch buffer
 * @param window_function windowing function and normalisiation factor to be applied to each input_samples_array element
 * @param window_function_length same length as input_samples_array_length and n_fft
 */
void
compute_power_spectrum_audio_samples(
    const audio_data_type* input_samples_array,
    const uint32_t input_samples_array_length,
    float* output_buffer,
    const uint32_t output_buffer_length,
    float* scratch_buffer,
    const uint32_t scratch_buffer_length,
    const float* window_function,
    const uint32_t window_function_length);

/**
 * Export the power spectrum from input_samples_array without needing to init
 * @param input_samples_array input samples array; will be converted from audio_data_type to float
 * @param n_fft same length as input_samples_array; must be a power of 2
 * @param output_buffer output buffer containing power spectrum; als used as intermediate step buffer
 * @param output_buffer_length must be (n_fft * 2). After computation, valid length is (n_fft / 2 + 1)
 * @param window_function windowing function and normalisiation factor to be applied to each input_samples_array element
 * @param window_function_length same length as n_fft
 */
void
compute_power_spectrum_audio_samples_direct(
    const audio_data_type* input_samples_array,
    const uint16_t n_fft,
    float* output_buffer,
    const uint32_t output_buffer_length,
    const float* window_function,
    const uint32_t window_function_length);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif //AUDIO_DSP_H
