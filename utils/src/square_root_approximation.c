#include "square_root_approximation.h"

#include <math.h>
#include <assert.h>

inline
float
square_root_approximation(
    const float target_value) {
    assert(target_value >= 0.0f);
    if (target_value == 0.0f) {
        return 0.0f;
    }

#ifdef __ARM_ARCH
    float output = 0.0f;

    arm_status status =
    arm_sqrt_f32(target_value, &output);
    assert(status == ARM_MATH_SUCCESS);

    return output;
#else
    // TODO: Investigate faster way to perform square root
    // For now, just use math library
    return sqrtf(target_value);
#endif // __ARM_ARCH
}
