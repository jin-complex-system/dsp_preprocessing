#ifndef POWER_TO_DECIBEL_H
#define POWER_TO_DECIBEL_H

#include <stdint.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compute in-place spectrogram to decibel units. Handles negative values
 *
 * Formula is roughly spectrogram_array[] = 10 * log10(spectrogram_array[]) - 10 * log10(reference_power)
 *
 * @param spectrogram_array
 * @param spectrogram_array_length
 * @param reference_power non-zero, positive value that is defined as 0 dB
 * @param top_decibel if non-zero and positive, clip values to top_decibel
 */
void
convert_power_to_decibel(
    float* spectrogram_array,
    const uint16_t spectrogram_array_length,
    const float reference_power,
    const float top_decibel);

/**
 * Compute in-place spectrogram to decibel units and scale to [0, 1]. Handles negative values
 *
 * Formula before scaling is roughly spectrogram_array[] = 10 * log10(spectrogram_array[]) - 10 * log10(reference_power)
 *
 * @param spectrogram_array
 * @param spectrogram_array_length
 * @param reference_power non-zero, positive value that is defined as 0 dB
 * @param top_decibel defined as the maximum of the dynamic range; clips values if necessary
 * @param min_decibel defined as the minimum of the dynamic range
 */
void
convert_power_to_decibel_and_scale(
    float* spectrogram_array,
    const uint16_t spectrogram_array_length,
    const float reference_power,
    const float top_decibel,
    const float min_decibel);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif

#endif //POWER_TO_DECIBEL_H
