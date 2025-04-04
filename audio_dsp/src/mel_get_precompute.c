#include "mel_spectrogram.h"
#include <assert.h>

/// n_mels = 32
#include <precomputed_mel/mel_centre_frequencies_float_32.h>
#include <precomputed_mel/mel_centre_frequencies_next_bin_32.h>
#include <precomputed_mel/mel_centre_frequencies_prev_bin_32.h>
#include <precomputed_mel/mel_frequency_weights_32.h>

/**
 * Assert that the precomputed buffer is as expected
 * @param precomputed_buffer
 * @param precomputed_buffer_length
 * @param expected_buffer_length
 */
static inline
void
assert_buffer(
    const void* precomputed_buffer,
    const uint32_t precomputed_buffer_length,
    const uint32_t expected_buffer_length)
{
#ifndef N_DEBUG
    assert(expected_buffer_length >= 1);
    assert(precomputed_buffer != NULL);
    assert(precomputed_buffer_length == expected_buffer_length);
#endif //N_DEBUG
}

/**
 * For n_mels = 32, get the precomputed values
 * @param n_mels
 * @param mel_centre_freq_float_buffer
 * @param mel_centre_freq_next_bin_buffer
 * @param mel_centre_freq_prev_bin_buffer
 * @param mel_freq_weights_buffer
 */
static inline
void
get_mel_spectrogram_precomputed_values_32(
    const uint16_t n_mels,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer) {
    /// Check parameters
    assert(n_mels == 32u);
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    /// Check precomputed
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_FLOAT_32,
        MEL_CENTRE_FREQUENCIES_FLOAT_32_LENGTH,
        n_mels + 1);
    assert_buffer(
    MEL_CENTRE_FREQUENCIES_NEXT_BIN_32,
    MEL_CENTRE_FREQUENCIES_NEXT_BIN_32_LENGTH,
    n_mels - 1);
    assert_buffer(
    MEL_CENTRE_FREQUENCIES_PREV_BIN_32,
    MEL_CENTRE_FREQUENCIES_PREV_BIN_32_LENGTH,
    n_mels - 1);
    assert_buffer(
    MEL_FREQUENCY_WEIGHTS_32,
    MEL_FREQUENCY_WEIGHTS_32_LENGTH,
    n_mels - 1);

    /// Assign values
    *mel_centre_freq_float_buffer = MEL_CENTRE_FREQUENCIES_FLOAT_32;
    *mel_centre_freq_next_bin_buffer = MEL_CENTRE_FREQUENCIES_NEXT_BIN_32;
    *mel_centre_freq_prev_bin_buffer = MEL_CENTRE_FREQUENCIES_PREV_BIN_32;
    *mel_freq_weights_buffer = MEL_FREQUENCY_WEIGHTS_32;
}

const
bool
get_mel_spectrogram_precomputed_values(
    const uint16_t n_mels,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer
) {
    /// Check parameters
    assert(n_mels >= 2);
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    /// Locate precomputed values
    bool found_precomputed_value = false;
    switch(n_mels)
    {
        default:
            break;
        case 32u:
            get_mel_spectrogram_precomputed_values_32(
                n_mels,
                mel_centre_freq_float_buffer,
                mel_centre_freq_next_bin_buffer,
                mel_centre_freq_prev_bin_buffer,
                mel_freq_weights_buffer
            );
            found_precomputed_value = true;
    }

#ifndef N_DEBUG
    if (found_precomputed_value)
    {
        assert(*mel_centre_freq_float_buffer != NULL);
        assert(*mel_centre_freq_next_bin_buffer != NULL);
        assert(*mel_centre_freq_prev_bin_buffer != NULL);
        assert(*mel_freq_weights_buffer != NULL);
    }
    else
    {
        assert(*mel_centre_freq_float_buffer == NULL);
        assert(*mel_centre_freq_next_bin_buffer == NULL);
        assert(*mel_centre_freq_prev_bin_buffer == NULL);
        assert(*mel_freq_weights_buffer == NULL);
    }

#endif // N_DEBUG

    return found_precomputed_value;
}


