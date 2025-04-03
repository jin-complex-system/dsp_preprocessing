#ifndef AUDIO_DSP_FFT_H
#define AUDIO_DSP_FFT_H

#include <stdint.h>

#ifdef __ARM_ARCH
#include "arm_math.h"
#endif //__ARM_ARCH

#ifdef TEST_ON_DESKTOP
#include <kiss_fft.h>
#include <kiss_fftr.h>
#endif //TEST_ON_DESKTOP

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARM_ARCH
typedef arm_rfft_fast_instance_f32 audio_dsp_rfft_instance;
#else
typedef kiss_fftr_cfg audio_dsp_rfft_instance;
#endif //__ARM_ARCH

/**
 * Initialise audio DSP Real FFT. Uses hardware DSP algorithm if possible
 * @param p_instance
 * @param n_fft must be a power of 2 value
 */
void
initialise_audio_dsp_rfft(
    audio_dsp_rfft_instance *p_instance,
    const uint16_t n_fft);

/**
 * Perform real FFT on input buffer with given audio_dsp_rfft_instance
 * @param p_instance
 * @param input_buffer real values of float
 * @param input_buffer_length same value as n_fft set during initialisation
 * @param output_buffer contains valid complex values of (input_buffer_length / 2) + 1
 * @param output_buffer_length must be input_buffer_length * 2
 */
void
perform_audio_dsp_rfft(
    audio_dsp_rfft_instance *p_instance,
    float* input_buffer,
    const uint32_t input_buffer_length,
    float* output_buffer,
    const uint32_t output_buffer_length);

/**
 * Perform real FFT on input buffer. Does initialisation and deinitialisation by itself
 * @param input_buffer real values of float
 * @param n_fft length of input buffer; must be power of 2
 * @param output_buffer contains valid complex values of (input_buffer_length / 2) + 1
 * @param output_buffer_length must be input_buffer_length * 2
 */
void
perform_audio_dsp_rfft_direct(
    float* input_buffer,
    const uint16_t n_fft,
    float* output_buffer,
    const uint32_t output_buffer_length);

/**
 * Deinitialise audio_dsp_rfft_instance
 * @param p_instance
 */
void
deinitialise_audio_dsp_rfft(
    audio_dsp_rfft_instance *p_instance);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif

#endif //AUDIO_DSP_FFT_H
