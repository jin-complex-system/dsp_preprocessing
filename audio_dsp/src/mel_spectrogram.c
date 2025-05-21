#include "mel_spectrogram.h"
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

/**
 * Get the mel centre freq float buffer length from n_mels
 * @param n_mels
 * @return
 */
static inline
uint16_t
get_mel_centre_freq_float_buffer_length(
    const uint16_t n_mels) {
    return n_mels + 2;
}

/**
 * Get the mel centre next/prev bin buffer length from n_mels
 * @param n_mels
 * @return
 */
static inline
uint16_t
get_mel_centre_next_prev_bin_buffer_length(
    const uint16_t n_mels) {
    return n_mels - 0;
}

/**
 * Get the mel weights buffer length from n_mels
 * @param n_mels
 * @return
 */
static inline
uint16_t
get_mel_freq_weights_buffer_length(
    const uint16_t n_mels) {
    return n_mels - 0;
}

/**
 * Get the minimum scratch buffer length
 *
 * Assuming it is float buffer and we are okay with gaps
 * @param n_mels
 * @return
 */
static inline
uint16_t
get_minimum_scratch_buffer_length(
    const uint16_t n_mels) {
    return
    get_mel_centre_freq_float_buffer_length(n_mels) +
    get_mel_centre_next_prev_bin_buffer_length(n_mels) +
    get_mel_centre_next_prev_bin_buffer_length(n_mels) +
    get_mel_freq_weights_buffer_length(n_mels);
}


void
compute_mel_spectrogram_bins(
    const uint16_t n_mels,
    const uint16_t n_fft,
    const uint16_t sample_rate,
    float* mel_centre_freq_float_buffer,
    uint16_t* mel_centre_freq_next_bin_buffer,
    uint16_t* mel_centre_freq_prev_bin_buffer,
    float* mel_freq_weights_buffer) {
    /// Check parameters
    assert(n_mels > 1);
    assert(n_fft % 2 == 0 && n_fft > 1);
    assert(sample_rate > 0.0);
    assert(mel_centre_freq_float_buffer != NULL);
    assert(mel_centre_freq_next_bin_buffer != NULL);
    assert(mel_centre_freq_prev_bin_buffer != NULL);
    assert(mel_freq_weights_buffer != NULL);

    /// Compute constants
    const float min_frequency = 0.0;
    const float max_frequency = sample_rate / 2;
    const float min_mel = convert_frequency_to_mel_slaney(min_frequency);
    const float max_mel = convert_frequency_to_mel_slaney(max_frequency);
    const uint16_t num_filterbanks = get_mel_centre_freq_float_buffer_length(n_mels);
    const uint16_t num_next_prev_bins = get_mel_centre_next_prev_bin_buffer_length(n_mels);
    const uint16_t num_weights = get_mel_freq_weights_buffer_length(n_mels);
    const float mel_step = (max_mel - min_mel) / num_filterbanks;
    const float freq_step = (float)(n_fft + 1) / (float)sample_rate;

    /// Get the mel-centred frequecies from mel-spaced frequencies
    for (uint16_t mel_iterator = 0; mel_iterator < (num_filterbanks - 1); mel_iterator++) {
        const float current_mel = min_mel + mel_iterator * mel_step;
        const float current_mel_spaced_freq = convert_mel_to_frequency_slaney(current_mel);
        assert(!isnan(current_mel_spaced_freq) && !isinf(current_mel_spaced_freq));

        const float frequency_bin = freq_step * current_mel_spaced_freq;

        assert(&mel_centre_freq_float_buffer[mel_iterator] != NULL);
        mel_centre_freq_float_buffer[mel_iterator] = frequency_bin;
    }
    assert(&mel_centre_freq_float_buffer[num_filterbanks - 1] != NULL);
    mel_centre_freq_float_buffer[num_filterbanks - 1] = max_frequency * freq_step;

    /// Get next and prev bin elements
    for (uint16_t bin_iterator = 0; bin_iterator < num_next_prev_bins; bin_iterator++) {
        const uint16_t prev_bin_iterator = bin_iterator + 0;
        const uint16_t next_bin_iterator = bin_iterator + 2;

        assert(&mel_centre_freq_float_buffer[prev_bin_iterator] != NULL);
        const uint16_t prev_index = (uint16_t)floor(mel_centre_freq_float_buffer[prev_bin_iterator]);

        assert(&mel_centre_freq_float_buffer[next_bin_iterator] != NULL);
        const uint16_t next_index = (uint16_t)ceil(mel_centre_freq_float_buffer[next_bin_iterator]);
        assert(next_index > prev_index);

        assert(&mel_centre_freq_prev_bin_buffer[bin_iterator] != NULL);
        mel_centre_freq_prev_bin_buffer[bin_iterator] = prev_index;

        assert(&mel_centre_freq_next_bin_buffer[bin_iterator] != NULL);
        mel_centre_freq_next_bin_buffer[bin_iterator] = next_index;
    }

    /// Calculate weights using Slaney method such that
    /// each mel has the approximate constant energy per channel
    for (uint16_t mel_iterator = 0; mel_iterator < num_weights; mel_iterator++) {
        const float weight = 2.0 / (
            mel_centre_freq_float_buffer[mel_iterator + 2] - mel_centre_freq_float_buffer[mel_iterator + 0]);
        assert(!isnan(weight) && !isinf(weight));

        assert(&mel_freq_weights_buffer[mel_iterator] != NULL);
        mel_freq_weights_buffer[mel_iterator] = weight;
    }
}

static
void
_compute_power_spectrum_into_mel_spectrogram(
    const float* power_spectrum_buffer,
    const uint16_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
    const uint16_t n_mels,
    const float* mel_centre_freq_float_buffer,
    const uint16_t* mel_centre_freq_next_bin_buffer,
    const uint16_t* mel_centre_freq_prev_bin_buffer,
    const float* mel_freq_weights_buffer) {
    /// Check parameters
    {
        assert(n_mels > 0 && power_spectrum_buffer_length > 0);
        assert(power_spectrum_buffer_length >= n_mels);
        assert(power_spectrum_buffer != NULL);
        assert(mel_spectrogram_buffer != NULL);
        assert(mel_centre_freq_float_buffer != NULL);
        assert(mel_centre_freq_next_bin_buffer != NULL);
        assert(mel_centre_freq_prev_bin_buffer != NULL);
        assert(mel_freq_weights_buffer != NULL);
    }

    /// Clear the output buffer
    memset(mel_spectrogram_buffer, 0, sizeof(float) * n_mels);
    for (uint32_t current_bin_index = 0; current_bin_index < n_mels; current_bin_index++) {

        /// Retrieve pre-computed values
        const float* prev_centre_filterbank = &mel_centre_freq_float_buffer[current_bin_index + 0];
        const float* current_centre_filterbank = &mel_centre_freq_float_buffer[current_bin_index + 1];
        const float* next_centre_filterbank = &mel_centre_freq_float_buffer[current_bin_index + 2];
        const float* weight = &mel_freq_weights_buffer[current_bin_index];

        const uint16_t next_bin_index = mel_centre_freq_next_bin_buffer[current_bin_index];
        const uint16_t prev_bin_index = mel_centre_freq_prev_bin_buffer[current_bin_index];
        assert(next_bin_index <= power_spectrum_buffer_length);

        float mel_spectrogram_value = 0.0f;
        for (uint16_t iterator_freq_bin = prev_bin_index; iterator_freq_bin < next_bin_index; iterator_freq_bin++) {
            const float iterator_freq_bin_float = iterator_freq_bin;
            assert(iterator_freq_bin <= power_spectrum_buffer_length);

            float weighted_power;
            /// Apply triangular filter
            if (
                iterator_freq_bin_float < *current_centre_filterbank &&
                iterator_freq_bin_float >= *prev_centre_filterbank) {
                weighted_power = (iterator_freq_bin_float - *prev_centre_filterbank) / (*current_centre_filterbank - *prev_centre_filterbank);
            }
            else if (
                iterator_freq_bin_float > *current_centre_filterbank &&
                iterator_freq_bin_float <= *next_centre_filterbank) {
                weighted_power = (*next_centre_filterbank - iterator_freq_bin_float) / (*next_centre_filterbank - *current_centre_filterbank);
            }
            /// Do not add the current weight
            else {
                continue;
            }

            /// Only assign weights if non-zero
            if (weighted_power == 0.0f) {
                continue;
            }
            assert(weighted_power != 0.0f);
            assert(
                !isinf(weighted_power) &&
                !isinf(weighted_power));

            mel_spectrogram_value += power_spectrum_buffer[iterator_freq_bin] * weighted_power;
        }

        mel_spectrogram_value = mel_spectrogram_value * *weight;
        mel_spectrogram_buffer[current_bin_index] = mel_spectrogram_value;
    }
}

void
compute_power_spectrum_into_mel_spectrogram_raw(
    const float* power_spectrum_buffer,
    const uint16_t power_spectrum_buffer_length,
    const uint16_t n_fft,
    const uint16_t sample_rate,
    float* mel_spectrogram_buffer,
    const uint16_t n_mel,
    float* scratch_buffer,
    const uint16_t scratch_buffer_length) {
    const uint16_t MINIMUM_SCRATCH_BUFFER_LENGTH = get_minimum_scratch_buffer_length(n_mel);

    /// Check parameters
    {
        assert(n_mel > 0 && power_spectrum_buffer_length > 0);
        assert(n_fft > 0 && n_fft % 2 == 0);
        assert(sample_rate > 0);
        assert(power_spectrum_buffer_length >= n_mel);
        assert(power_spectrum_buffer != NULL);
        assert(mel_spectrogram_buffer != NULL);
        assert(scratch_buffer_length >= MINIMUM_SCRATCH_BUFFER_LENGTH && scratch_buffer != NULL);
    }

    /// Compute constants
    const uint16_t mel_centre_start_iter = 0;
    const uint16_t mel_next_start_iter = get_mel_centre_freq_float_buffer_length(n_mel) + mel_centre_start_iter;
    const uint16_t mel_prev_start_iter = get_mel_centre_next_prev_bin_buffer_length(n_mel) + mel_next_start_iter;
    const uint16_t mel_weights_start_iter = get_mel_centre_next_prev_bin_buffer_length(n_mel) + mel_prev_start_iter;

    float* mel_centre_freq_float_buffer = &scratch_buffer[mel_centre_start_iter];
    uint16_t* mel_centre_freq_next_bin_buffer = (uint16_t*)&scratch_buffer[mel_next_start_iter];
    uint16_t* mel_centre_freq_prev_bin_buffer = (uint16_t*)&scratch_buffer[mel_prev_start_iter];
    float* mel_freq_weights_buffer = &scratch_buffer[mel_weights_start_iter];

    compute_mel_spectrogram_bins(
        n_mel,
        n_fft,
        sample_rate,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer
    );

    _compute_power_spectrum_into_mel_spectrogram(
        power_spectrum_buffer,
        power_spectrum_buffer_length,
        mel_spectrogram_buffer,
        n_mel,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer
    );
}

void
compute_power_spectrum_into_mel_spectrogram(
    const float* power_spectrum_buffer,
	const uint16_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
    const uint16_t n_fft,
    const uint16_t sample_rate,
	const uint16_t n_mel) {
	/// Check parameters
	{
	    assert(n_mel > 0 && power_spectrum_buffer_length > 0);
	    assert(n_fft > 0 && n_fft % 2 == 0);
	    assert(sample_rate > 0);
		assert(power_spectrum_buffer_length >= n_mel);
	    assert(power_spectrum_buffer != NULL);
	    assert(mel_spectrogram_buffer != NULL);
	}

    const float* mel_centre_freq_float_buffer = NULL;
    const uint16_t* mel_centre_freq_next_bin_buffer = NULL;
    const uint16_t* mel_centre_freq_prev_bin_buffer = NULL;
    const float* mel_freq_weights_buffer = NULL;

    const bool
    found_precomputed_values = get_mel_spectrogram_precomputed_values(
            n_mel,
            n_fft,
            sample_rate,
            &mel_centre_freq_float_buffer,
            &mel_centre_freq_next_bin_buffer,
            &mel_centre_freq_prev_bin_buffer,
            &mel_freq_weights_buffer
        );
    assert(found_precomputed_values);
    assert(mel_centre_freq_float_buffer != NULL);
    assert(mel_centre_freq_next_bin_buffer != NULL);
    assert(mel_centre_freq_prev_bin_buffer != NULL);
    assert(mel_freq_weights_buffer != NULL);

    _compute_power_spectrum_into_mel_spectrogram(
        power_spectrum_buffer,
        power_spectrum_buffer_length,
        mel_spectrogram_buffer,
        n_mel,
        mel_centre_freq_float_buffer,
        mel_centre_freq_next_bin_buffer,
        mel_centre_freq_prev_bin_buffer,
        mel_freq_weights_buffer
    );
}
