#include <gtest/gtest.h>

#include <cstdint>
#include <insertion_sort.h>

TEST(InsertionSort, Base) {
    float unsorted_array[] = {
        0.0f, 1.0f, -1.0f, -2.0f, 0.5f, 2.0f, -2.0f, -0.5f, -0.1f,
    };
    constexpr auto NUM_ELEMENTS = sizeof(unsorted_array) / sizeof(float);

    float sorted_array[NUM_ELEMENTS];

    /// Perform insertion sort
    for (uint32_t iterator = 0; iterator < NUM_ELEMENTS; iterator++) {
        insertion_sort(
            unsorted_array[iterator],
            sorted_array,
            iterator
        );
    }

    /// Check
    for (uint32_t first_iterator = 0; first_iterator < NUM_ELEMENTS; first_iterator++) {
        for (uint32_t second_iterator = first_iterator; second_iterator < NUM_ELEMENTS; second_iterator++) {
            if (first_iterator != second_iterator) {
                EXPECT_LE(sorted_array[first_iterator], sorted_array[second_iterator]);
            }
        }
    }
}
