#include "convert_complex.h"
#include "log_approximation.h"
#include "square_root_approximation.h"
#include <math.h>
#include <assert.h>

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


float
compute_magnitude_from_complex(
    const float real_value,
    const float img_value,
    const float scale_factor) {
    assert(!isinf(real_value) && !isnan(real_value));
    assert(!isinf(img_value) && !isnan(img_value));

    /// If both components are zero, just return 0
    if (real_value == 0.0f && img_value == 0.0f) {
        return 0.0f;
    }

    /// If either components are zero,
    /// just return the absolute value of the other
    ///
    /// For this project, the hope is
    /// imaginary component is more likely to be zero
    if (img_value == 0.0f) {
        return fabsf(real_value);
    }
    if (real_value == 0.0f) {
        return fabsf(img_value);
    }

    // Computationally expensive but necessary case
    const float power = real_value * real_value + img_value * img_value + MINIMUM_FLOAT_VALUE;
    return square_root_approximation(power) * scale_factor;
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
