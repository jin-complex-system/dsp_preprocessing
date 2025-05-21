#include "mel_spectrogram.h"
#include <assert.h>
#include <stddef.h>

/// Generated library headers
#include <precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_22048.h>
#include <precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_44100.h>
#include <precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_44100.h>

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

/// Generated functions
static inline
void
get_mel_precomputed_values_mel_64_nfft_1024_sr_22048(
    const uint16_t n_mel,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer) {
    /// Check parameters
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    assert_buffer(
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_64_FFT_1024_SR_22048_BUFFER,
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_64_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel + 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_64_FFT_1024_SR_22048_BUFFER,
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_64_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_64_FFT_1024_SR_22048_BUFFER,
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_64_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_FREQUENCY_WEIGHTS_MEL_64_FFT_1024_SR_22048_BUFFER,
        MEL_FREQUENCY_WEIGHTS_MEL_64_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel - 1);
    /// Assign values
    *mel_centre_freq_float_buffer = MEL_CENTRE_FREQUENCIES_FLOAT_MEL_64_FFT_1024_SR_22048_BUFFER;
    *mel_centre_freq_next_bin_buffer = MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_64_FFT_1024_SR_22048_BUFFER;
    *mel_centre_freq_prev_bin_buffer = MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_64_FFT_1024_SR_22048_BUFFER;
    *mel_freq_weights_buffer = MEL_FREQUENCY_WEIGHTS_MEL_64_FFT_1024_SR_22048_BUFFER;
}

static inline
void
get_mel_precomputed_values_mel_32_nfft_1024_sr_22048(
    const uint16_t n_mel,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer) {
    /// Check parameters
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    assert_buffer(
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_32_FFT_1024_SR_22048_BUFFER,
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_32_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel + 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_32_FFT_1024_SR_22048_BUFFER,
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_32_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_32_FFT_1024_SR_22048_BUFFER,
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_32_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_FREQUENCY_WEIGHTS_MEL_32_FFT_1024_SR_22048_BUFFER,
        MEL_FREQUENCY_WEIGHTS_MEL_32_FFT_1024_SR_22048_BUFFER_LENGTH,
        n_mel - 1);
    /// Assign values
    *mel_centre_freq_float_buffer = MEL_CENTRE_FREQUENCIES_FLOAT_MEL_32_FFT_1024_SR_22048_BUFFER;
    *mel_centre_freq_next_bin_buffer = MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_32_FFT_1024_SR_22048_BUFFER;
    *mel_centre_freq_prev_bin_buffer = MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_32_FFT_1024_SR_22048_BUFFER;
    *mel_freq_weights_buffer = MEL_FREQUENCY_WEIGHTS_MEL_32_FFT_1024_SR_22048_BUFFER;
}

static inline
void
get_mel_precomputed_values_mel_64_nfft_1024_sr_44100(
    const uint16_t n_mel,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer) {
    /// Check parameters
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    assert_buffer(
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_64_FFT_1024_SR_44100_BUFFER,
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_64_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel + 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_64_FFT_1024_SR_44100_BUFFER,
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_64_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_64_FFT_1024_SR_44100_BUFFER,
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_64_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_FREQUENCY_WEIGHTS_MEL_64_FFT_1024_SR_44100_BUFFER,
        MEL_FREQUENCY_WEIGHTS_MEL_64_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel - 1);
    /// Assign values
    *mel_centre_freq_float_buffer = MEL_CENTRE_FREQUENCIES_FLOAT_MEL_64_FFT_1024_SR_44100_BUFFER;
    *mel_centre_freq_next_bin_buffer = MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_64_FFT_1024_SR_44100_BUFFER;
    *mel_centre_freq_prev_bin_buffer = MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_64_FFT_1024_SR_44100_BUFFER;
    *mel_freq_weights_buffer = MEL_FREQUENCY_WEIGHTS_MEL_64_FFT_1024_SR_44100_BUFFER;
}

static inline
void
get_mel_precomputed_values_mel_32_nfft_1024_sr_44100(
    const uint16_t n_mel,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer) {
    /// Check parameters
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    assert_buffer(
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_32_FFT_1024_SR_44100_BUFFER,
        MEL_CENTRE_FREQUENCIES_FLOAT_MEL_32_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel + 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_32_FFT_1024_SR_44100_BUFFER,
        MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_32_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_32_FFT_1024_SR_44100_BUFFER,
        MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_32_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel - 1);
    assert_buffer(
        MEL_FREQUENCY_WEIGHTS_MEL_32_FFT_1024_SR_44100_BUFFER,
        MEL_FREQUENCY_WEIGHTS_MEL_32_FFT_1024_SR_44100_BUFFER_LENGTH,
        n_mel - 1);
    /// Assign values
    *mel_centre_freq_float_buffer = MEL_CENTRE_FREQUENCIES_FLOAT_MEL_32_FFT_1024_SR_44100_BUFFER;
    *mel_centre_freq_next_bin_buffer = MEL_CENTRE_FREQUENCIES_NEXT_BIN_MEL_32_FFT_1024_SR_44100_BUFFER;
    *mel_centre_freq_prev_bin_buffer = MEL_CENTRE_FREQUENCIES_PREV_BIN_MEL_32_FFT_1024_SR_44100_BUFFER;
    *mel_freq_weights_buffer = MEL_FREQUENCY_WEIGHTS_MEL_32_FFT_1024_SR_44100_BUFFER;
}

const
bool
get_mel_spectrogram_precomputed_values(
    const uint16_t n_mel,
    const uint16_t n_fft,
    const uint16_t sample_rate,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer
) {
    /// Check parameters
    assert(n_mel >= 2 && n_mel % 2 == 0);
    assert(n_fft > 0 && n_fft % 2 == 0);
    assert(sample_rate > 0);
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    /// Locate precomputed values
    bool found_precomputed_value = false;

    /// Generated if conditionals
if (
    n_mel == 64u &&
    n_fft == 1024u &&
    sample_rate == 22048u) {

    get_mel_precomputed_values_mel_64_nfft_1024_sr_22048(
        n_mel,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer);
    found_precomputed_value = true;
}

if (
    n_mel == 32u &&
    n_fft == 1024u &&
    sample_rate == 22048u) {

    get_mel_precomputed_values_mel_32_nfft_1024_sr_22048(
        n_mel,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer);
    found_precomputed_value = true;
}

if (
    n_mel == 64u &&
    n_fft == 1024u &&
    sample_rate == 44100u) {

    get_mel_precomputed_values_mel_64_nfft_1024_sr_44100(
        n_mel,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer);
    found_precomputed_value = true;
}

if (
    n_mel == 32u &&
    n_fft == 1024u &&
    sample_rate == 44100u) {

    get_mel_precomputed_values_mel_32_nfft_1024_sr_44100(
        n_mel,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer);
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
