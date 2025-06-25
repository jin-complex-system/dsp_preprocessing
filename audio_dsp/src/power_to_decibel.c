#include "power_to_decibel.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

#define MINIUMUM_SUPPORTED_POWER        _get_minimum_power()
#define MINIMUM_SUPPORTED_DECIBEL       _get_minimum_decibel()

/**
 * Vector power to decibel conversion. Optimised
 *
 * Formula is pDestination[n] = 10.0f * log10(pSource[n])
 *
 * @param pSource
 * @param pDestination
 * @param numElements
 */
static inline
void
v_power_to_decibel_conversion(
    const float* pSource,
    float* pDestination,
    const uint32_t numElements) {
    /// Check parameters
    {
        assert(pSource != NULL);
        assert(pDestination != NULL);
        assert(numElements > 0);
    }

    for (uint32_t iterator = 0; iterator < numElements; iterator++) {
        assert(&pDestination[iterator] != NULL);
        assert(&pSource[iterator] != NULL);

        // pDestination[iterator] = 10.0f * log10_approximation(pSource[iterator]);
        v_loge_approximation(
            &pSource[iterator],
            &pDestination[iterator],
            1
        );
        pDestination[iterator] = pDestination[iterator] * _get_power_to_decibel_constant();

        assert(!isnan(pDestination[iterator]) && !isinf(pDestination[iterator]));
    }
}

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
        assert(top_decibel >= 0.0f || top_decibel == -1.0f);

        assert(!isnan(reference_power) && !isinf(reference_power));
        assert(!isnan(top_decibel) && !isinf(top_decibel));
    }

    /// Compute constants
    float
    reference_log = 0.0f;
    if (reference_power != 1.0f) {
        v_power_to_decibel_conversion(
            &reference_power,
            &reference_log,
            1
        );
        assert(reference_log != -0.0f);
    }

    /// Iterate through the entire spectrogram
    for (uint16_t iterator = 0; iterator < spectrogram_array_length; iterator++) {
        assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));

        /// Handle negative and zero values
        if (spectrogram_array[iterator] <= MINIUMUM_SUPPORTED_POWER) {
            spectrogram_array[iterator] = MINIMUM_SUPPORTED_DECIBEL - reference_log;
        }
        /// If power matches to reference_power, this is 0.0 dB
        else if (spectrogram_array[iterator] == reference_power) {
            spectrogram_array[iterator] = 0.0f;
        }
        else {
            v_power_to_decibel_conversion(
                &spectrogram_array[iterator],
                &spectrogram_array[iterator],
                1
            );
            spectrogram_array[iterator] = spectrogram_array[iterator] - reference_log;
        }

        /// Clip top decibel if top decibel is positive or zero
        if (top_decibel >= 0.0f && spectrogram_array[iterator] > top_decibel) {
            spectrogram_array[iterator] = top_decibel;
        }
        assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));
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

        assert(!isnan(reference_power) && !isinf(reference_power));
        assert(!isnan(top_decibel) && !isinf(top_decibel));
    }

    /// Compute constants
    const float
    decibel_range = top_decibel - min_decibel;
    const float
    zero_decibel_scaled = (MINIMUM_SUPPORTED_DECIBEL - top_decibel) / decibel_range;
    float
    reference_log = 0.0f;
    if (reference_power != 1.0f) {
        v_power_to_decibel_conversion(
            &reference_power,
            &reference_log,
            1
        );
    }

    /// Iterate through the entire spectrogram
    for (uint16_t iterator = 0; iterator < spectrogram_array_length; iterator++) {
        assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));

        /// Handle negative and zero values
        if (spectrogram_array[iterator] <= MINIUMUM_SUPPORTED_POWER) {
            spectrogram_array[iterator] = zero_decibel_scaled;
        }
        else if (spectrogram_array[iterator] == reference_power) {
            spectrogram_array[iterator] = zero_decibel_scaled;
        }
        else
        {
            float decibel_value;
            v_power_to_decibel_conversion(
                &decibel_value,
                &spectrogram_array[iterator],
                1
            );
            decibel_value -= decibel_value - reference_log;

            /// Clip maximum
            if (decibel_value > top_decibel) {
                spectrogram_array[iterator] = 1.0f;
            }
            /// Clip minimum
            else if (decibel_value < min_decibel) {
                spectrogram_array[iterator] = 0.0f;
            }
            else {
                spectrogram_array[iterator] = (decibel_value - top_decibel) / decibel_range;
            }
        }
    }

    // TODO: Finish implementing this function
    assert( 0 == 1 );
}
