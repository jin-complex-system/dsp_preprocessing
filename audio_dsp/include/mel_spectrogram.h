#ifndef MEL_SPECTROGRAM_H_
#define MEL_SPECTROGRAM_H_

#include <stdint.h>
#include <stdbool.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * For a given frequency, compute the equivalent mels using HTK method
 *
 * Formula is 2595 * log10(1 + frequency / 700)
 *
 * @param frequency
 * @return mels as a float
 */
float
convert_frequency_to_mel_htk(
	const float frequency);

/**
 * For a given frequency, compute the equivalent mels using Slaney method
 *
 * For frequency < 1000, use the formula 3 * frequency / 200
 * For frequency >= 1000, use the formula 15 + 27 * log6.4(frequency / 1000)
 *
 * @param frequency
 * @return mels as a float
 */
float
convert_frequency_to_mel_slaney(
	const float frequency);

/**
 * For a given mel, compute the equivalent frequency back using htk method
 *
 * Formula is frequency = 700 * ((exp(mel / 1125)) - 1)
 *
 * @param mel
 * @return frequency as a float
 */
float
convert_mel_to_frequency_htk(
	const float mel);

/**
 * For a given mel, compute the equivalent frequency back using Slaney method
 *
 * Formula is frequency = 700 * ((exp(mel / 1125)) - 1)
 *
 * @param mel
 * @return frequency as a float
 */
float
convert_mel_to_frequency_slaney(
	const float mel);

/**
 * For a given parameters, get the precomputed values if available
 * @param n_mel
 * @param n_fft
 * @param sample_rate sample rate of the audio signal
 * @param max_frequency the range of mel bins up to sample_rate / 2; if set to 0, use sample_rate / 2
 * @param mel_centre_freq_float_buffer
 * @param mel_centre_freq_next_bin_buffer
 * @param mel_centre_freq_prev_bin_buffer
 * @param mel_freq_weights_buffer
 * @return true if all the buffers are set; false otherwise
 */
const
bool
get_mel_spectrogram_precomputed_values(
	const uint16_t n_mel,
	const uint16_t n_fft,
	const uint16_t sample_rate,
	const uint16_t max_frequency,
	const float** mel_centre_freq_float_buffer,
	const uint16_t** mel_centre_freq_next_bin_buffer,
	const uint16_t** mel_centre_freq_prev_bin_buffer,
	const float** mel_freq_weights_buffer
);

/**
 * For a given n_mels and sample rate, compute the values for the buffers using Slaney method
 *
 * Minimum frequency is 0.0 for all intents and purposes
 *
 * Not recommended for repeated operations
 *
 * @param n_mel
 * @param n_fft number of FFT bins; must be power of 2
 * @param sample_rate sample rate of the audio signal
 * @param max_frequency the range of mel bins up to sample_rate / 2; if set to 0, use sample_rate / 2
 * @param mel_centre_freq_float_buffer
 * @param mel_centre_freq_next_bin_buffer
 * @param mel_centre_freq_prev_bin_buffer
 * @param mel_freq_weights_buffer
 */
void
compute_mel_spectrogram_bins(
	const uint16_t n_mel,
	const uint16_t n_fft,
	const uint16_t sample_rate,
	const uint16_t max_frequency,
	float* mel_centre_freq_float_buffer,
	uint16_t* mel_centre_freq_next_bin_buffer,
	uint16_t* mel_centre_freq_prev_bin_buffer,
	float* mel_freq_weights_buffer);

/**
 * Convert power spectrum into mel spectrogram. Compute necessary parameters
 *
 * Will compute the necessary mel freq if needed
 * @param power_spectrum_buffer
 * @param power_spectrum_buffer_length
 * @param n_fft
 * @param sample_rate sample rate of the audio signal
 * @param max_frequency the range of mel bins up to sample_rate / 2; if set to 0, use sample_rate / 2
 * @param mel_spectrogram_buffer
 * @param n_mel must be power_spectrum_buffer_length or less
 * @param scratch_buffer store the computed values
 * @param scratch_buffer_length length of at least n_mels * 4 - 2)
 * @return max value of mel spectrogram
 */
float
compute_power_spectrum_into_mel_spectrogram_raw(
	const float* power_spectrum_buffer,
	const uint16_t power_spectrum_buffer_length,
	const uint16_t n_fft,
	const uint16_t sample_rate,
	const uint16_t max_frequency,
	float* mel_spectrogram_buffer,
	const uint16_t n_mel,
	float* scratch_buffer,
	const uint16_t scratch_buffer_length);

/**
 * Convert power spectrum into mel spectrogram.
 *
 * Will compute the necessary mel freq if needed unless specified otherwise
 * @param power_spectrum_buffer
 * @param power_spectrum_buffer_length
 * @param mel_spectrogram_buffer
 * @param n_fft
 * @param sample_rate sample rate of the audio signal
 * @param max_frequency the range of mel bins up to sample_rate / 2; if set to 0, use sample_rate / 2
 * @param n_mel must be power_spectrum_buffer_length or less
 * @return max value of mel spectrogram
 */
float
compute_power_spectrum_into_mel_spectrogram(
    const float* power_spectrum_buffer,
	const uint16_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
	const uint16_t n_fft,
	const uint16_t sample_rate,
	const uint16_t max_frequency,
	const uint16_t n_mel);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif

#endif // MEL_SPECTROGRAM_H_
