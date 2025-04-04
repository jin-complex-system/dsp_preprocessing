#include "mel_spectrogram.h"
#include <assert.h>

void
compute_mel_spectrogram_bins(
    const uint16_t n_mels,
    float* mel_centre_freq_float_buffer,
    uint16_t* mel_centre_freq_next_bin_buffer,
    uint16_t* mel_centre_freq_prev_bin_buffer,
    float* mel_freq_weights_buffer
) {
    /// Check parameters
    assert(n_mels > 1);
    assert(mel_centre_freq_float_buffer != NULL);
    assert(mel_centre_freq_next_bin_buffer != NULL);
    assert(mel_centre_freq_prev_bin_buffer != NULL);
    assert(mel_freq_weights_buffer != NULL);

    // TODO: Implement this function
}
