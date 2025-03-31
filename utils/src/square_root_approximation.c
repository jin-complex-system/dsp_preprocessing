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

    // TODO: Investigate faster way to perform square root
    // For now, just use math library
    return sqrtf(target_value);

}
