#include "convert_complex.h"
#include "log_approximation.h"
#include "square_root_approximation.h"
#include <math.h>
#include <assert.h>
#include <stddef.h>

const
float
MINIMUM_FLOAT_VALUE = 1.175494351e-38f;

const
float
MINIMUM_LOG2_APPROXIMATION = -126.0f;

/**
 * General formula for computing logarithm from complex numbers
 *
 * Formula is log_factor * log2_approximation(magnituded_squared) + log_scale_factor
 *
 * @param real_value
 * @param img_value
 * @param log_factor non-zero, precomputed value
 * @param log_scale_factor factor such that magnitude = sqrtf((real * real + img * img) * 2 ^ (log_scale_factor)
 * @return
 */
static inline
float
_compute_log_from_complex(
    const float real_value,
    const float img_value,
    const float log_factor,
    const float log_scale_factor) {
    /// Check parameters
    assert(log_factor != 0.0f);
    assert(!isinf(log_factor) && !isnan(log_factor));

    assert(!isinf(log_scale_factor) && !isnan(log_scale_factor));

    assert(!isinf(real_value) && !isnan(real_value));
    assert(!isinf(img_value) && !isnan(img_value));

    float magnituded_squared = MINIMUM_FLOAT_VALUE;

    /// If both components are zero, just return 0
    if (real_value == 0.0f && img_value == 0.0f) {
        return log_factor * MINIMUM_LOG2_APPROXIMATION + log_scale_factor;
    }
    else {
        float real_squared = real_value * real_value;
        if (isinf(real_squared) || isnan(real_squared)) {
            real_squared = 0.0f;
        }
        assert(real_squared >= 0.0f);
        assert(!isinf(real_squared) && !isnan(real_squared));

        float img_squared = img_value * img_value;

        if (isinf(img_squared) || isnan(img_squared)) {
            img_squared = 0.0f;
        }
        assert(img_squared >= 0.0f);
        assert(!isinf(img_squared) && !isnan(img_squared));


        if (real_squared == 0.0f && img_squared == 0.0f) {
            return log_factor * MINIMUM_LOG2_APPROXIMATION + log_scale_factor;
        }
        else {
            magnituded_squared += real_squared + img_squared;
        }
        assert(magnituded_squared > MINIMUM_FLOAT_VALUE);
        assert(!isinf(magnituded_squared) && !isnan(magnituded_squared));
    }

    return log_factor * log2_approximation(magnituded_squared) + log_scale_factor;
}

/**
 * Compute power from complex number while being hardware-agnostic
 *
 * Assumes that precision loss is acceptable, and
 * comparison and fabs() operations are overall cheaper to run
 * @param p_real_value pointer to the input value that is a real value
 * @param p_img_value pointer to the input value that is an imaginary value
 * @return power of complex number
 */
static inline
float
_compute_power_from_complex(
    const float* p_real_value,
    const float* p_img_value) {
    assert(p_real_value != NULL && p_img_value != NULL);
    assert(!isinf(*p_real_value) && !isnan(*p_real_value));
    assert(!isinf(*p_img_value) && !isnan(*p_img_value));

    /// If both components are zero, just return 0
    if (*p_real_value == 0.0f && *p_img_value == 0.0f) {
        return 0.0f;
    }
    /// If one component is 0.0f and the other abs(1.0), is just return 1.0f
    else if (
        (*p_real_value == 0.0f && *p_img_value == 1.0f) ||
        (*p_real_value == 0.0f && *p_img_value == -1.0f) ||
        (*p_img_value == 0.0f && *p_real_value == 1.0f) ||
        *p_img_value == 0.0f && *p_real_value == -1.0f) {
        return 1.0f;
    }

    // Computationally expensive but necessary case
    const float power =
        *p_real_value * *p_real_value + *p_img_value * *p_img_value + MINIMUM_FLOAT_VALUE;
    return power;
}

/**
 * Compute magnitude from complex number while being hardware-agnostic
 *
 * Assumes that precision loss is acceptable, and
 * comparison and fabs() operations are overall cheaper to run
 * @param p_real_value pointer to the input value that is a real value
 * @param p_img_value pointer to the input value that is an imaginary value
 * @return magnitude of complex number
 */
static inline
float
_compute_magnitude_from_complex(
    const float* p_real_value,
    const float* p_img_value) {
    assert(p_real_value != NULL && p_img_value != NULL);
    assert(!isinf(*p_real_value) && !isnan(*p_real_value));
    assert(!isinf(*p_img_value) && !isnan(*p_img_value));

    /// If either components are zero,
    /// just return the absolute value of the other
    ///
    /// For this project, the hope is
    /// imaginary component is more likely to be zero
    if (*p_img_value == 0.0f) {
        return fabsf(*p_real_value);
    }
    else if (*p_real_value == 0.0f) {
        return fabsf(*p_img_value);
    }

    const float power =
        _compute_power_from_complex(
            p_real_value,
            p_img_value);
    return square_root_approximation(power);
}

void
compute_power_from_complex_arrays(
    const float* complex_input_vector,
    float* output_array,
    const uint32_t num_samples) {
#ifdef __ARM_ARCH
    arm_cmplx_mag_squared_f32(
        complex_input_vector,
        output_array,
        num_samples);
#else
    for (uint32_t iterator = 0; iterator < num_samples; iterator++) {
        const float* p_real_value = &complex_input_vector[iterator * 2 + 0];
        const float* p_img_value = &complex_input_vector[iterator * 2 + 1];
        assert(p_real_value != NULL);
        assert(p_img_value != NULL);
        assert(&output_array[iterator] != NULL);

        output_array[iterator] = _compute_power_from_complex(
            p_real_value,
            p_img_value);
    }
#endif // __ARM_ARCH
}

void
compute_magnitude_from_complex_arrays(
    const float* complex_input_vector,
    float* output_array,
    const uint32_t num_samples) {
    /// Check parameters
    assert(complex_input_vector != NULL && output_array != NULL);
    assert(num_samples >= 1u);
#ifdef __ARM_ARCH
    arm_cmplx_mag_f32(
        complex_input_vector,
        output_array,
        num_samples);

#else
    for (uint32_t iterator = 0; iterator < num_samples; iterator++) {
        const float* p_real_value = &complex_input_vector[iterator * 2 + 0];
        const float* p_img_value = &complex_input_vector[iterator * 2 + 1];
        assert(p_real_value != NULL);
        assert(p_img_value != NULL);
        assert(&output_array[iterator] != NULL);

        output_array[iterator] = _compute_magnitude_from_complex(
            p_real_value,
            p_img_value);
    }
#endif // __ARM_ARCH
}

float
compute_log_from_complex(
    const float real_value,
    const float img_value,
    const float log_scale_factor) {
    const float log_factor = 1.0f / (2.0f * log2f(10.0f));
    return _compute_log_from_complex(
        real_value,
        img_value,
        log_factor,
        log_scale_factor);
}

float
compute_decibels_from_complex(
    const float real_value,
    const float img_value,
    const float log_scale_factor) {
    const float log_factor = 10.0f / log2f(10.0f);
    return _compute_log_from_complex(
        real_value,
        img_value,
        log_factor,
        log_scale_factor);
}
