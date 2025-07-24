#ifndef AUDIO_PREPROCESS_H
#define AUDIO_PREPROCESS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List of parameters for audio_preprocess
 */
struct audio_preprocess_parameters {
    uint16_t n_fft;
    uint16_t hop_length;
    uint16_t n_mels;

    uint16_t sample_rate;
    uint16_t num_seconds;

    uint16_t max_frequency;

    /// Cropping
    uint32_t num_cropped_frames;      // Set to 0 if not used
    bool left_padding;              // If false, do centre cropping
};

/**
 * Setup module audio_preprocess
 * @param pParameters pointer object to valid parameters; throws assert() if invalid
 */
void
audio_preprocess_setup(
    const struct audio_preprocess_parameters* pParameters);

/**
 * Deinitialise module audio_preprocess
 */
void
audio_preprocess_deinit(void);

/**
 * Perform computation of audio DSP. Presumes audio_preprocess_parameters is initialised
 *
 * Proposed approach to perform DSP on an audio buffer. Supports left padding
 *
 * @param audio_input_buffer
 * @param audio_input_buffer_length length of the audio_input_buffer
 * @param num_valid_audio_elements number of valid audio elements inside audio_input_buffer
 * @param power_spectrum_buffer
 * @param power_spectrum_buffer_length
 * @param mel_spectrogram_buffer output is returned here as (uint8_t*)mel_spectrogram_buffer
 * @param mel_spectrogram_buffer_length
 */
void
audio_preprocess_compute(
    const int16_t* audio_input_buffer,
    const uint32_t audio_input_buffer_length,
    const uint32_t num_valid_audio_elements,
    float* power_spectrum_buffer,
    const uint32_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
    const uint32_t mel_spectrogram_buffer_length);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif

#endif //AUDIO_PREPROCESS_H
