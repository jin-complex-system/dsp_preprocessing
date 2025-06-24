#include "mel_spectrogram.h"
#include <assert.h>
#include <log_approximation.h>
#include <stddef.h>

static const
float
zero_log = 10.0f * -37.929779052734375f;

void
convert_power_to_decibel(
    float* spectrogram_array,
    const uint16_t spectrogram_array_length,
    const float reference_power,
    const float top_decibel) {
    /// Check parameters
    {
        assert(spectrogram_array_length > 0);
        assert(spectrogram_array != NULL);
        assert(reference_power > 0.0f);
        assert(top_decibel > 0.0f || top_decibel == -1.0f);
    }

    /// Compute constants
    float
    reference_log = 0.0f;
    if (reference_power != 1.0f) {
        reference_log = 10.0f * log10_approximation(reference_power);
    }

    /// Iterate through the entire spectrogram
    for (uint16_t iterator = 0; iterator < spectrogram_array_length; iterator++) {
        /// Handle negative and zero values
        if (spectrogram_array[iterator] <= 0.0f) {
            spectrogram_array[iterator] = zero_log - reference_log;
        }
        else if (spectrogram_array[iterator] == reference_power) {
            spectrogram_array[iterator] = 0.0f;
        }
        else {
            spectrogram_array[iterator] =
                10.0f * log10_approximation(spectrogram_array[iterator]) - reference_log;

            /// Clip the top decibel
            if (top_decibel != -1.0f && spectrogram_array[iterator] > top_decibel) {
                spectrogram_array[iterator] = top_decibel;
            }
        }
    }
}

void
convert_power_to_decibel_and_scale(
    float* spectrogram_array,
    const uint16_t spectrogram_array_length,
    const float reference_power,
    const float top_decibel,
    const float min_decibel) {
    /// Check parameters
    {
        assert(spectrogram_array_length > 0);
        assert(spectrogram_array != NULL);
        assert(reference_power > 0.0f);
        assert(top_decibel > min_decibel);
    }

    /// Compute constants
    const float
    decibel_range = top_decibel - min_decibel;
    const float
    zero_decibel_scaled = (zero_log - top_decibel) / decibel_range;
    float
    reference_log = 0.0f;
    if (reference_power != 1.0f) {
        reference_log = 10.0f * log10_approximation(reference_power);
    }

    /// Iterate through the entire spectrogram
    for (uint16_t iterator = 0; iterator < spectrogram_array_length; iterator++) {
        /// Handle negative and zero values
        if (spectrogram_array[iterator] <= 0.0f) {
            spectrogram_array[iterator] = zero_decibel_scaled;
        }
        else if (spectrogram_array[iterator] == reference_power) {
            spectrogram_array[iterator] = zero_decibel_scaled;
        }
        else
        {
            const float decibel_value =
                10.0f * log10_approximation(spectrogram_array[iterator]) - reference_log;

            /// Clip maximum
            if (decibel_value > top_decibel) {
                spectrogram_array[iterator] = 1.0f;
            }
            /// Clip minimum
            else if (decibel_value < min_decibel) {
                spectrogram_array[iterator] = 0.0f;
            }
            else {
                spectrogram_array[iterator] = (decibel_value - top_decibel) / decibel_range;\
            }
        }
    }

    // TODO: Finish implementing this function
    assert(false);
}
