#include "mel_spectrogram.h"
#include <assert.h>
#include <math.h>
#include <log_approximation.h>

static const float
MINIMUM_FREQUENCY_SLANEY = 1000.0f; // Frequency at 15.0 mel
static const float
MINIMUM_MEL_SLANEY = 15.0f; // Mel at 1000 Hz

float
convert_frequency_to_mel_htk(
    const float frequency) {
    /// Check parameters
    assert(frequency > 0.0f);

    /// Original formula is 2595 * log(1 + frequency / 700)
    /// Optimised to 2595 * log10(frequency + 700) - freq_to_mel_constant, where
    /// freq_to_mel_constant = 2595 * log10(700)
    const float freq_to_mel_constant = 7383.02941383699635480297729372978210449218750f;

    float log_value = log10_approximation(frequency + 700.0f);
    assert(!isnan(log_value) && !isinf(log_value));

    return 2595.0f * log_value - freq_to_mel_constant;
}

float
convert_frequency_to_mel_slaney(
    const float frequency) {
    /// Check parameters
    assert(frequency > 0.0f);

    /// Linear - Original formula is 3 * frequency / 200 or frequency * 0.015
    if (frequency < MINIMUM_FREQUENCY_SLANEY) {
        return frequency * 0.015f;
    }

    /// Log-Scaler
    /// Original formula is MINIMUM_MEL_SLANEY + 27 * log 6.4 (frequency / MINIMUM_FREQUENCY_SLANEY)
    /// Optimised to first_constant * log10(frequency) + second_constant, where
    ///   first_constant = 27 / (log10 6.4),
    ///   second_constant = MINIMUM_MEL_SLANEY - 27 * log10(1000) / log10(6.4)

    const float first_constant = 33.4912809438499365910502092447131872177124023437500000000f;
    const float second_constant = -85.4738428315498168785779853351414203643798828125000000000f;

    float log_value = log10_approximation(frequency);
    assert(!isnan(log_value) && !isinf(log_value));

    return first_constant * log_value + second_constant;
}

float
convert_mel_to_frequency_htk(
    const float mel) {
    /// Check parameters
    assert(mel > 0.0f);

    // TODO: Optimise the following
    return 700.0f * (expf(mel / 1127.0f) - 1.0f);
}

float
convert_mel_to_frequency_slaney(
    const float mel) {
    /// Check parameters
    assert(mel > 0.0f);

    /// Linear
    /// Original formula is 200 * mel / 3
    if (mel < MINIMUM_MEL_SLANEY) {
        return 66.6666666666666714036182384006679058074951171875000000000f * mel;
    }

    /// Log-Scaler part
    /// Original formula is MINIMUM_FREQUENCY_SLANEY * exp( log_step * (mel - MINIMUM_MEL_SLANEY))
    return MINIMUM_FREQUENCY_SLANEY * expf(
        0.0687517774209491228099011550511932000517845153808593750f * (mel - MINIMUM_MEL_SLANEY));

    // TODO: Verify the following optimisation
    /// Optimised to constant * exp(mel) where
    ///   constant = MINIMUM_FREQUENCY_SLANEY * exp ( log_step) / exp (log_step * MINIMUM_MEL_SLANEY)
    // return 381.9273453463964642651262693107128143310546875000000000000f * expf(mel);

}
