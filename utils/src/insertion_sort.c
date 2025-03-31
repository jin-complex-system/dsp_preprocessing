#include "insertion_sort.h"

void
insertion_sort(
    const float target_value,
    float* sorted_array,
    const uint32_t array_length) {
  /// Insert immediately at the front
  if (array_length == 0) {
    sorted_array[0] = target_value;
    return;
  }
  /// Insert at the end
  else if (sorted_array[array_length - 1] <= target_value) {
    sorted_array[array_length] = target_value;
    return;
  }
  else {
    float displaced_value = target_value;
    for (uint32_t iterator = 0; iterator < array_length; iterator++) {
      if (sorted_array[iterator] > displaced_value) {
        float temp_value = sorted_array[iterator];
        sorted_array[iterator] = displaced_value;
        displaced_value = temp_value;
      }
    }
    sorted_array[array_length] = displaced_value;
    return;
  }
}