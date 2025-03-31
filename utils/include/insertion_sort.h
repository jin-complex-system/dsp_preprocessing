#ifndef INSERTION_SORT_H
#define INSERTION_SORT_H

#include <stdint.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Conduct insertion sort into a sorted array
 * @param target_value
 * @param sorted_array
 * @param array_length non-zero length of the array
 */
void
insertion_sort(
    const float target_value,
    float* sorted_array,
    const uint32_t array_length);

  /* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //INSERTION_SORT_H
