#include "log_approximation.h"
#include <math.h>
#include <assert.h>

const
float
MINIMUM_LOG10_VALUE = -37.929779052734375f;

const
float
MINIMUM_LOG2_VALUE = -126.0f;

inline
float
log10_approximation(const float target_value) {
    assert(target_value >= 0.0f);
    assert(!isinf(target_value) && !isnan(target_value));

    if (target_value == 0.0f) {
        return MINIMUM_LOG10_VALUE;
    }

    // TODO: Investigate faster way to perform log10
    // For now, just use math library
    return log10f(target_value);
}

inline
float
log2_approximation(const float target_value) {
    assert(target_value >= 0.0f);
    assert(!isinf(target_value) && !isnan(target_value));

    if (target_value == 0.0f) {
        return MINIMUM_LOG2_VALUE;
    }

    // TODO: Investigate faster way to perform log2
    // For now, just use math library
    return log2f(target_value);
}
