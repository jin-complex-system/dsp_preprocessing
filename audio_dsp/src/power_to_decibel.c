#include "power_to_decibel.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>

#ifdef __ARM_ARCH
#include "arm_math.h"
#endif //__ARM_ARCH

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

    v_loge_approximation(
        pSource,
        pDestination,
        numElements);

#ifdef __ARM_ARCH
    arm_scale_f32(
        pDestination,
        _get_power_to_decibel_constant(),
        pDestination,
        numElements
    );
#else
    for (uint32_t iterator = 0; iterator < numElements; iterator++) {
        assert(&pDestination[iterator] != NULL);
        assert(!isnan(pDestination[iterator]) && !isinf(pDestination[iterator]));

        pDestination[iterator] = pDestination[iterator] * _get_power_to_decibel_constant();
        assert(!isnan(pDestination[iterator]) && !isinf(pDestination[iterator]));
    }
#endif //__ARM_ARCH
}

void
convert_power_to_decibel(
    float* spectrogram_array,
    const uint32_t spectrogram_array_length,
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
    for (uint32_t iterator = 0; iterator < spectrogram_array_length; iterator++) {
        /// Handles NaN and Inf
        /// Also cleans up spectrogram_array[]
        if (isnan(spectrogram_array[iterator]) || isinf(spectrogram_array[iterator])) {
            spectrogram_array[iterator] = 0.0f;
            continue;
        }
        assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));

        /// Handle negative and zero values
        if (spectrogram_array[iterator] <= MINIUMUM_SUPPORTED_POWER) {
            spectrogram_array[iterator] = MINIMUM_SUPPORTED_DECIBEL - reference_log;
        }
        /// If power matches to reference_power, this is 0.0 dB by definition
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
    uint8_t* output_buffer,
    const uint32_t num_elements,
    const float reference_power) {
    const float MAX_DECIBEL = _power_to_decibel_get_max_decibel();
    const float MIN_DECIBEL = _power_to_decibel_get_min_decibel();

    /// Check parameters
    {
        assert(num_elements > 0);
        assert(output_buffer != NULL);
        assert(spectrogram_array != NULL);
        assert(reference_power >= 0.0f);

        assert(!isnan(reference_power) && !isinf(reference_power));
        assert(MAX_DECIBEL > MIN_DECIBEL);
        assert(UINT8_MAX == 255u);
    }

    /// Original function would be:
    /// output_buffer[n] = (uint8_t)( 255u * ( scaled_decibel - MIN_DECIBEL ) / ( MAX_DECIBEL - MIN_DECIBEL) )
    /// where
    ///   scaled_decibel = ln( spectrogram_array[n] ) * ( 10.0 / ln(10.0) ) -  ln( reference_power ),
    ///   and 0.5f is used to round to the nearest integer
    ///
    /// This can be rearranged to:
    /// output_buffer[n] = (uint8_t)( ( ln(spectrogram_array[n]) * decibel_constant - reference_decibel * ln_constant - scaling_constant )
    /// where
    ///     reference_decibel - ln ( reference_power)
    ///     decibel_constant - 10.0 / ln( 10.0),
    ///     ln_constant - 255u / (MAX_DECIBEL - MIN_DECIBEL), and
    ///     scaling_constant - 255 * (MIN_DECIBEL) / (MAX_DECIBEL - MIN_DECIBEL)

    // const float decibel_constant = _get_power_to_decibel_constant(); // 10.0 / ln(10.0),
    const float ln_constant = (float)UINT8_MAX / (MAX_DECIBEL - MIN_DECIBEL);
    const float scaling_constant = (UINT8_MAX * (MIN_DECIBEL)) / (MAX_DECIBEL - MIN_DECIBEL);

    float
    reference_decibel = MAX_DECIBEL;
    if (reference_power != 1.0f) {
        v_power_to_decibel_conversion(
            &reference_power,
            &reference_decibel,
            1
        );
        assert(reference_decibel != -0.0f);
    }

    /// Iterate through the entire spectrogram
    for (uint32_t iterator = 0; iterator < num_elements; iterator++) {
        /// Handles NaN and Inf
        /// Also cleans up spectrogram_array[]
        if (isnan(spectrogram_array[iterator]) || isinf(spectrogram_array[iterator])) {
            spectrogram_array[iterator] = MINIUMUM_SUPPORTED_POWER;
            output_buffer[iterator] = 0;
            continue;
        }
        assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));

        /// If power matches or is greater than reference_power, this is 0 dB by definition
        /// which is basically the max decibel
        if (spectrogram_array[iterator] >= reference_power) {
            assert(MAX_DECIBEL == 0.0f);
            output_buffer[iterator] = (uint8_t)UINT8_MAX;
            continue;
        }

        /// Handle negative and zero values
        /// just set to MINIMUM_SUPPORTED_DECIBEL
        else if (spectrogram_array[iterator] <= MINIUMUM_SUPPORTED_POWER) {
            spectrogram_array[iterator] = MINIMUM_SUPPORTED_DECIBEL - reference_decibel;
        }

        /// Compute ln
        else {
            v_power_to_decibel_conversion(
                &spectrogram_array[iterator],
                &spectrogram_array[iterator],
                1
            );
            spectrogram_array[iterator] = spectrogram_array[iterator] - reference_decibel;
            assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));
        }

        /// Clip to 255 if reach MAX_DECIBEL
        if (spectrogram_array[iterator] >= MAX_DECIBEL) {
            output_buffer[iterator] = (uint8_t)UINT8_MAX;
        }
        /// Clip to 0 if reach MIN_DECIBEL
        else if (spectrogram_array[iterator] <= MIN_DECIBEL) {
            output_buffer[iterator] = 0;
        }
        /// Find the range in-between
        else {
            spectrogram_array[iterator] = spectrogram_array[iterator] * ln_constant - scaling_constant;
            assert(!isnan(spectrogram_array[iterator]) && !isinf(spectrogram_array[iterator]));

            /// Clip to UINT8_MAX
            if (spectrogram_array[iterator] >= (float)UINT8_MAX) {
                output_buffer[iterator] = (uint8_t)UINT8_MAX;
            }
            /// Clip to 0
            else if (spectrogram_array[iterator] <= 0.0f) {
                output_buffer[iterator] = 0;
            }
            else
            {
                output_buffer[iterator] = (uint8_t)spectrogram_array[iterator];
            }
        }
    }
}
