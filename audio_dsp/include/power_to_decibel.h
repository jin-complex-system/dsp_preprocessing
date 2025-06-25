#ifndef POWER_TO_DECIBEL_H
#define POWER_TO_DECIBEL_H

#include <stdint.h>
#include <log_approximation.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the power to decibel constant 10 / loge(10.0), where loge(x) * 10 / loge(10.0)
 *
 * @return 10.0 / loge(10.0)
 */
static inline
const float
_get_power_to_decibel_constant(void) {
    return 4.3429448190325175f; // 10.0 / loge(10.0);
}

/**
 * Get the lowest supported power for convert_power_to_decibel()
 * @return
 */
static inline
const float
_get_minimum_power(void) {
    return get_log_approximation_minimum_supported_value();
}

/**
 * Get the lowest supported decibel for convert_power_to_decibel().
 *
 * @return 10 * loge() / loge(10.0)
 */
static inline
const float
_get_minimum_decibel(void) {
    return _get_loge_approximation_minimum_value() * _get_power_to_decibel_constant();
}

/**
 * Compute in-place spectrogram to decibel units. Handles negative values
 *
 * Formula is roughly spectrogram_array[] = 10 * log10(spectrogram_array[]) - 10 * log10(reference_power)
 *
 * @param spectrogram_array
 * @param spectrogram_array_length
 * @param reference_power non-zero, positive value that is defined as 0 dB
 * @param top_decibel clip values to top_decibel (positive and zero values). -1.0f if not used
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
