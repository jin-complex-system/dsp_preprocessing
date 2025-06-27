#include "log_approximation.h"
#include <math.h>
#include <assert.h>
#include <stddef.h>

inline
float
log10_approximation(const float target_value) {
    assert(!isinf(target_value) && !isnan(target_value));

    if (target_value <= get_log_approximation_minimum_supported_value()) {
        return _get_log10_approximation_minimum_value();
    }
    else {
        assert(target_value > get_log_approximation_minimum_supported_value());

        // TODO: Investigate faster way to perform log10
        // For now, just use math library
        return log10f(target_value);
    }
}

inline
float
log2_approximation(const float target_value) {
    assert(!isinf(target_value) && !isnan(target_value));
    if (target_value <= get_log_approximation_minimum_supported_value()) {
        return _get_log2_approximation_minimum_value();
    }
    else {
        assert(target_value > get_log_approximation_minimum_supported_value());

        // TODO: Investigate faster way to perform log2
        // For now, just use math library
        return log2f(target_value);

    }
}

inline
float
loge_approximation(const float target_value) {
    assert(!isinf(target_value) && !isnan(target_value));
    if (target_value <= get_log_approximation_minimum_supported_value()) {
        return _get_loge_approximation_minimum_value();
    }
    else {
        assert(target_value > get_log_approximation_minimum_supported_value());

        // TODO: Investigate faster way to perform log
        // For now, just use math library
        return logf(target_value);
    }
}

#ifdef __ARM_ARCH
inline
#endif // __ARM_ARCH
void
v_loge_approximation(
    const float* pSource,
    float* pDestination,
    const uint32_t numElements) {
    /// Check parameters
    {
        assert(pSource != NULL);
        assert(pDestination != NULL);
        assert(numElements > 0);
    }

#ifdef __ARM_ARCH
    arm_clip_f32(
        pSource,
        pDestination,
        MINIMUM_FLOAT_VALUE,
        3.4028235 × 1e38,
        numElements,
    );
    arm_vlog_f32(
        pDestination,
        pDestination,
        numElements
    );
    // TODO: Check if we need to sanitise the output
    arm_clip_f32(
        pSource,
        pDestination,
        MINIMUM_FLOAT_VALUE,
        3.4028235 × 1e38,
        numElements,
    );
#else
    for (uint32_t iterator = 0; iterator < numElements; iterator++) {
        assert(&pSource[iterator] != NULL);
        assert(&pDestination[iterator] != NULL);
        pDestination[iterator] = loge_approximation(pSource[iterator]);
    }
#endif // __ARM_ARCH
}
