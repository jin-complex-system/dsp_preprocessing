#include "mel_spectrogram.h"
#include <assert.h>
#include <math.h>
#include <log_approximation.h>

float
convert_frequency_to_mel(
    const float frequency) {
    /// Check parameters
    assert(frequency > 0.0f);

    /// Original formula is 1125 * log(1 + frequency / 700)
    /// Optimised to 1125 * log(frequency + 700) - freq_to_mel_constant
    /// where freq_to_mel_constant = 1125 * log(700)
    const float freq_to_mel_constant = 7369.9653769238302629673853516578674316406250000000000000000f;

    float log_value = log10_approximation(frequency + 700);
    assert(!isnan(log_value) && !isinf(log_value));

    return 1125.0f * log_value - freq_to_mel_constant;
}

float
convert_mel_to_frequency(
    const float mel) {
    /// Check parameters
    assert(mel > 0.0f);

    // TODO: Optimise the following
    return 700.0f * (expf(mel / 1125.0f) - 1.0f);
}
