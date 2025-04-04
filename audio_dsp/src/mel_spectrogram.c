#include "mel_spectrogram.h"
#include <assert.h>
#include <math.h>
#include <string.h>

void
compute_power_spectrum_into_mel_spectrogram(
    const float* power_spectrum_buffer,
	const uint16_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
	const uint16_t n_mels) {
	/// Check parameters
	{
	    assert(n_mels > 0 && power_spectrum_buffer_length > 0);
		assert(power_spectrum_buffer_length >= n_mels);
	    assert(power_spectrum_buffer != NULL);
	    assert(mel_spectrogram_buffer != NULL);
	}

    float* mel_centre_freq_float_buffer = NULL;
    uint16_t* mel_centre_freq_next_bin_buffer = NULL;
    uint16_t* mel_centre_freq_prev_bin_buffer = NULL;
    float* mel_freq_weights_buffer = NULL;

    const bool
    found_precomputed_values = get_mel_spectrogram_precomputed_values(
        n_mels,
        &mel_centre_freq_float_buffer,
        &mel_centre_freq_next_bin_buffer,
        &mel_centre_freq_prev_bin_buffer,
        &mel_freq_weights_buffer
    );

    if (!found_precomputed_values)
    {
        assert(found_precomputed_values); // only support precomputed values at this time

        compute_mel_spectrogram_bins(
            n_mels,
            mel_centre_freq_float_buffer,
            mel_centre_freq_next_bin_buffer,
            mel_centre_freq_prev_bin_buffer,
            mel_freq_weights_buffer
        );
    }

    assert(mel_centre_freq_float_buffer != NULL);
    assert(mel_centre_freq_next_bin_buffer != NULL);
    assert(mel_centre_freq_prev_bin_buffer != NULL);
    assert(mel_freq_weights_buffer != NULL);

    memset(mel_spectrogram_buffer, 0, sizeof(float) * n_mels);

    for (uint32_t mel_iterator = 1; mel_iterator < n_mels; mel_iterator++) {
        const uint16_t current_bin_index = mel_iterator - 1;
        assert(current_bin_index < n_mels - 1);

        /// Retrieve pre-computed values
        const float* prev_centre_filterbank = &mel_centre_freq_float_buffer[mel_iterator - 1];
        const float* current_centre_filterbank = &mel_centre_freq_float_buffer[mel_iterator];
        const float* next_centre_filterbank = &mel_centre_freq_float_buffer[mel_iterator + 1];
        const float* weight = &mel_freq_weights_buffer[current_bin_index];

        const uint16_t next_bin_index = mel_centre_freq_next_bin_buffer[current_bin_index];
        const uint16_t prev_bin_index = mel_centre_freq_prev_bin_buffer[current_bin_index];
        assert(next_bin_index > prev_bin_index && next_bin_index <= power_spectrum_buffer_length);

        float mel_spectrogram_value = 0.0f;
        for (uint16_t iterator_freq_bin = prev_bin_index; iterator_freq_bin <= next_bin_index; iterator_freq_bin++) {
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

            /// Assign weights if non-zero
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



