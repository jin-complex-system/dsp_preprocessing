#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdint.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

struct
statistics
{
    float max;
    float min;
    float mean;
    float median;
    float variance;
    // float first_der_mean;
    // float first_der_variance;
};

#define STATISTICS_ELEMENT_LENGTH                   (sizeof(struct statistics)/sizeof(float))

/**
 * Compute statistics from input_buffer acrosss different frames
 * @param statistics_input_buffer input buffer of length num_frames * bin_length
 * @param num_frames number of frames
 * @param bin_length number of bins
 * @param frame_difference_reciprocal non-zero, positive difference between each frame per bin as a reciprocal
 * @param statistics_output_buffer output buffer containing struct statistics
 * @param statistics_output_buffer_length length * bin_length
 * @param scratch_buffer scratch buffer
 * @param scratch_buffer_length length of at least num_frames * 2
 */
void
compute_statistics_across_frames(
    const float* statistics_input_buffer,
    const uint32_t num_frames,
    const uint32_t bin_length,
    const float frame_difference_reciprocal,
    struct statistics* statistics_output_buffer,
    const uint32_t statistics_output_buffer_length,
    float* scratch_buffer,
    const uint32_t scratch_buffer_length);

/**
 * Compute statistics from input_buffer within one frame
 * @param statistics_input_buffer
 * @param bin_length number of bins
 * @param bin_difference_reciprocal non-zero, positive difference between each bin as a reciprocal
 * @param statistics_output_buffer struct statistics
 * @param statistics_output_buffer_length length of bin_length
 * @param scratch_buffer
 * @param scratch_buffer_length length of at least bin_length * 2
 */
void
compute_statistics_within_frame(
    const float* statistics_input_buffer,
    const uint32_t bin_length,
    const float bin_difference_reciprocal,
    struct statistics* statistics_output_buffer,
    const uint32_t statistics_output_buffer_length,
    float* scratch_buffer,
    const uint32_t scratch_buffer_length);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif

#endif //STATISTICS_H
