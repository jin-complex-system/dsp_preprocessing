#include "statistics.h"

#include <assert.h>
#include <string.h>
#include <insertion_sort.h>
#include <square_root_approximation.h>

/**
 * Prepare current_stats_pointer before start computing statistics
 * @param current_stats_pointer
 * @param statistics_output_buffer
 * @param output_buffer_index
 */
static inline
void
statistics_initialise_stats_pointer(
    struct statistics* current_stats_pointer) {
    assert(current_stats_pointer != NULL);

    current_stats_pointer->max = -9999.0f;
    current_stats_pointer->min = 9999.0f;
    current_stats_pointer->mean = 0.0f;
    current_stats_pointer->median = 0.0f;
    current_stats_pointer->variance = 0.0f;
    // current_stats_pointer->first_der_mean = 0.0f;
    // current_stats_pointer->first_der_variance = 0.0f;
}

/**
 * Used for first for-loop calculation
 * @param input_buffer
 * @param current_index
 * @param sorted_buffer_pointer
 * @param first_loop_iterator
 * @param current_stats_pointer
 * @param difference_reciprocal
 * @param first_derivative_pointer
 */
static inline
void
statistics_compute_first_loop(
    const float* input_buffer,
    const uint32_t current_index,
    float* sorted_buffer_pointer,
    const uint32_t first_loop_iterator,
    struct statistics* current_stats_pointer,
    const float difference_reciprocal,
    float* first_derivative_pointer,
    const uint32_t previous_index) {
    /// Check parameters
    assert(input_buffer != NULL);
    assert(sorted_buffer_pointer != NULL);
    assert(current_stats_pointer != NULL);
    assert(first_derivative_pointer != NULL);
    assert(first_loop_iterator <= current_index);
    assert(first_loop_iterator == 0 || previous_index < current_index);

    const float current_value = input_buffer[current_index];

    if (current_value > current_stats_pointer->max) {
        current_stats_pointer->max = current_value;
    }
    if (current_stats_pointer->min > current_value) {
        current_stats_pointer->min = current_value;
    }

    current_stats_pointer->mean += current_value;

    insertion_sort(
        current_value,
        sorted_buffer_pointer,
        first_loop_iterator
    );

    // if (first_loop_iterator >= 1) {
    //     assert(previous_index < current_index);
    //     const float first_derivative_value =
    //         (current_value - input_buffer[previous_index]) * difference_reciprocal;
    //     first_derivative_pointer[first_loop_iterator - 1] = first_derivative_value;
    //
    //     current_stats_pointer->first_der_mean += first_derivative_value;
    // }
}

/**
 * Used to calculate mean
 * @param current_stats_pointer
 * @param total_number_of_elements
 * @param first_derivative_length
 */
static inline
void
statistics_compute_means(
    struct statistics* current_stats_pointer,
    const uint32_t total_number_of_elements,
    const uint32_t first_derivative_length) {
    /// Check parameters
    assert(current_stats_pointer != NULL);
    assert(total_number_of_elements > 0);
    assert(first_derivative_length == total_number_of_elements - 1);

    /// Compute mean
    current_stats_pointer->mean =
        current_stats_pointer->mean / (float)total_number_of_elements;
    // current_stats_pointer->first_der_mean =
    //     current_stats_pointer->first_der_mean / (float)first_derivative_length;
}

static inline
void
statistics_locate_median(
    struct statistics* current_stats_pointer,
    const float* sorted_buffer_pointer,
    const uint32_t sorted_buffer_length) {
    /// Check parameters
    assert(sorted_buffer_length > 1);

    if (sorted_buffer_length & 0b1) {
        const uint32_t ODD_MEDIAN_ELEMENT_IN_SORTED_BUFFER = (sorted_buffer_length)/2 + 1;

        current_stats_pointer->median =
            sorted_buffer_pointer[ODD_MEDIAN_ELEMENT_IN_SORTED_BUFFER];
    }
    /// Find median in even INPUT_BUFFER_LENGTH
    else{
        const uint32_t ONE_OF_EVEN_MEDIAN_ELEMENT_IN_SORTED_BUFFER = (sorted_buffer_length)/2;

        current_stats_pointer->median =
            (sorted_buffer_pointer[ONE_OF_EVEN_MEDIAN_ELEMENT_IN_SORTED_BUFFER] + sorted_buffer_pointer[ONE_OF_EVEN_MEDIAN_ELEMENT_IN_SORTED_BUFFER - 1]) / 2;
    }
}

/**
 * Used for second for-loop calculation
 * @param statistics_input_buffer
 * @param current_index
 * @param second_loop_iterator
 * @param first_derivative_buffer
 * @param current_stats_pointer
 */
static inline
void
statistics_compute_second_loop(
    const float* statistics_input_buffer,
    const uint32_t current_index,
    const uint32_t second_loop_iterator,
    const float* first_derivative_buffer,
    struct statistics* current_stats_pointer) {
    /// Check parameters
    assert(first_derivative_buffer != NULL);
    assert(statistics_input_buffer != NULL);
    assert(current_stats_pointer != NULL);
    assert(second_loop_iterator <= current_index);

    const float* current_value_pointer =
        &statistics_input_buffer[current_index];

    // /// Compute first derivative's variance
    // if (second_loop_iterator >= 1) {
    //     const float first_derivative_variance =
    //         (first_derivative_buffer[second_loop_iterator - 1] - current_stats_pointer->first_der_mean) *
    //             (first_derivative_buffer[second_loop_iterator - 1] - current_stats_pointer->first_der_mean);
    //
    //     if (first_derivative_variance >= 0.0f) {
    //         current_stats_pointer->first_der_variance += first_derivative_variance;
    //     }
    // }

    const float variance =
        (*current_value_pointer - current_stats_pointer->mean) * (*current_value_pointer - current_stats_pointer->mean);
    if (variance >= 0.0f) {
        current_stats_pointer->variance += variance;
    }
}

static inline
void
statistics_compute_variance(
    struct statistics* current_stats_pointer,
    const uint32_t total_number_of_elements,
    const uint32_t first_derivative_length) {
    /// Check parameters
    assert(current_stats_pointer != NULL);
    assert(total_number_of_elements > 0);
    assert(first_derivative_length == total_number_of_elements - 1);

    assert(current_stats_pointer->variance >= 0.0f);
    current_stats_pointer->variance =
        current_stats_pointer->variance / (float)total_number_of_elements;
    current_stats_pointer->variance =
        square_root_approximation(current_stats_pointer->variance);

    // assert(current_stats_pointer->first_der_variance >= 0.0f);
    // current_stats_pointer->first_der_variance =
    //     current_stats_pointer->first_der_variance / (float)first_derivative_length;
    // current_stats_pointer->first_der_variance =
    //     square_root_approximation(current_stats_pointer->first_der_variance);
}

void
compute_statistics_across_frames(
    const float* statistics_input_buffer,
    const uint32_t num_frames,
    const uint32_t bin_length,
    const float frame_difference_reciprocal,
    struct statistics* statistics_output_buffer,
    const uint32_t statistics_output_buffer_length,
    float* scratch_buffer,
    const uint32_t scratch_buffer_length) {
    /// Helpful constants
    const uint32_t INPUT_BUFFER_LENGTH = bin_length * num_frames;
    const uint32_t FIRST_DERIVATIVE_LENGTH = num_frames - 1;
    const uint32_t SORTED_BUFFER_LENGTH = num_frames;

    /// Check parameters
    assert(bin_length > 1 && num_frames > 1);
    assert(frame_difference_reciprocal > 0.0f);
    assert(scratch_buffer_length >= SORTED_BUFFER_LENGTH + FIRST_DERIVATIVE_LENGTH);
    assert(statistics_output_buffer_length == bin_length);

    /// Set the statistics output
    memset(
        statistics_output_buffer,
        0,
        statistics_output_buffer_length * STATISTICS_ELEMENT_LENGTH * sizeof(float));

    for (uint32_t bin_iterator = 0; bin_iterator < bin_length; bin_iterator++) {
        /// Prepare scratch buffer
        float* first_derivative_buffer_pointer =
            &scratch_buffer[0];
        float* sorted_buffer_pointer =
            &scratch_buffer[FIRST_DERIVATIVE_LENGTH];

        /// Prepare statistics
        struct statistics*
        current_stats_pointer = (struct statistics*)&statistics_output_buffer[bin_iterator];
        statistics_initialise_stats_pointer(current_stats_pointer);

        /// First for-loop
        for (uint32_t frame_iterator = 0; frame_iterator < num_frames; frame_iterator++) {
            const uint32_t current_input_index = bin_iterator + frame_iterator * bin_length;
            const uint32_t previous_input_index = bin_iterator + (frame_iterator - 1) * bin_length;
            assert(current_input_index < INPUT_BUFFER_LENGTH);

            statistics_compute_first_loop(
                statistics_input_buffer,
                current_input_index,
                sorted_buffer_pointer,
                frame_iterator,
                current_stats_pointer,
                frame_difference_reciprocal,
                first_derivative_buffer_pointer,
                previous_input_index
            );
        }

        /// Compute mean
        statistics_compute_means(
            current_stats_pointer,
            num_frames,
            FIRST_DERIVATIVE_LENGTH
        );

        /// Locate median
        statistics_locate_median(
            current_stats_pointer,
            sorted_buffer_pointer,
            SORTED_BUFFER_LENGTH
        );

        /// Second for-loop
        for (uint32_t frame_iterator = 0; frame_iterator < num_frames; frame_iterator++) {
            const uint32_t current_input_index = bin_iterator + frame_iterator * bin_length;
            assert(current_input_index < INPUT_BUFFER_LENGTH);

            statistics_compute_second_loop(
                statistics_input_buffer,
                current_input_index,
                frame_iterator,
                first_derivative_buffer_pointer,
                current_stats_pointer);
        }

        statistics_compute_variance(
            current_stats_pointer,
            num_frames,
            FIRST_DERIVATIVE_LENGTH);
    }
}

void
compute_statistics_within_frame(
    const float* statistics_input_buffer,
    const uint32_t bin_length,
    const float bin_difference_reciprocal,
    struct statistics* statistics_output_buffer,
    const uint32_t statistics_output_buffer_length,
    float* scratch_buffer,
    const uint32_t scratch_buffer_length) {
    /// Helpful constants
    const uint32_t FIRST_DERIVATIVE_LENGTH = bin_length - 1;
    const uint32_t SORTED_BUFFER_LENGTH = bin_length;

    /// Check parameters
    assert(bin_length > 1);
    assert(scratch_buffer_length >= SORTED_BUFFER_LENGTH + FIRST_DERIVATIVE_LENGTH);
    assert(statistics_output_buffer_length >= 1);
    assert(bin_difference_reciprocal > 0.0f);

    /// Set the statistics output
    memset(
        statistics_output_buffer,
        0,
        statistics_output_buffer_length * STATISTICS_ELEMENT_LENGTH * sizeof(float));

    /// Prepare scratch buffer
    float* first_derivative_buffer_pointer =
        &scratch_buffer[0];
    float* sorted_buffer_pointer =
        &scratch_buffer[FIRST_DERIVATIVE_LENGTH];

    /// Prepare statistics
    struct statistics*
    current_stats_pointer = (struct statistics*)statistics_output_buffer;
    statistics_initialise_stats_pointer(current_stats_pointer);

    /// First for-loop
    for (uint32_t bin_iterator = 0; bin_iterator < bin_length; bin_iterator++) {
        const uint32_t current_input_index = bin_iterator;
        const uint32_t previous_input_index = current_input_index - 1;

        statistics_compute_first_loop(
            statistics_input_buffer,
            current_input_index,
            sorted_buffer_pointer,
            bin_iterator,
            current_stats_pointer,
            bin_difference_reciprocal,
            first_derivative_buffer_pointer,
            previous_input_index
        );
    }

    /// Compute mean
    statistics_compute_means(
        current_stats_pointer,
        bin_length,
        FIRST_DERIVATIVE_LENGTH
    );

    /// Locate median
    statistics_locate_median(
        current_stats_pointer,
        sorted_buffer_pointer,
        SORTED_BUFFER_LENGTH
    );

    /// Second for-loop
    for (uint32_t bin_iterator = 0; bin_iterator < bin_length; bin_iterator++) {
        const uint32_t current_input_index = bin_iterator;

        statistics_compute_second_loop(
            statistics_input_buffer,
            current_input_index,
            bin_iterator,
            first_derivative_buffer_pointer,
            current_stats_pointer);
    }

    statistics_compute_variance(
        current_stats_pointer,
        bin_length,
        FIRST_DERIVATIVE_LENGTH);
}
