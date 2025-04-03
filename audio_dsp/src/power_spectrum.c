#include "power_spectrum.h"

#include <assert.h>
#include <convert_complex.h>
#include "audio_dsp_fft.h"

static
audio_dsp_rfft_instance
power_spectrum_rfft;

static
uint16_t
n_fft_set = 0;

static
float*
fft_input;

static
float*
fft_output;

const
float
POWER_SPECTRUM_LOG_SCALE_FACTOR = 0.0f;

void
initialise_power_spectrum(
    const uint16_t n_fft) {
    /// Check parameters
    assert(n_fft % 2 == 0 && n_fft >= 2);

    initialise_audio_dsp_rfft(
        &power_spectrum_rfft,
        n_fft);
    n_fft_set = n_fft;
}

void
deinit_power_spectrum(void) {
    deinitialise_audio_dsp_rfft(&power_spectrum_rfft);
    n_fft_set = 0;
}

bool
compute_power_spectrum_audio_samples(
    const audio_data_type* input_samples_array,
    const uint32_t input_samples_array_length,
    float* output_buffer,
    const uint32_t output_buffer_length,
    float* scratch_buffer,
    const uint32_t scratch_buffer_length,
    const float power_threshold,
    const float* window_function,
    const uint32_t window_function_length) {
    /// Compute constants
    const uint32_t RFFT_OUTPUT_LENGTH = n_fft_set / 2 + 1;
    const uint32_t FFT_INPUT_LENGTH = input_samples_array_length * 1;
    const uint32_t FFT_OUTPUT_LENGTH = input_samples_array_length * 2; // Includes real and imaginary values

    /// Check parameters
    {
    	assert(FFT_INPUT_LENGTH == n_fft_set);
    }

    /// No scratch buffer; use output buffer as scratch buffer
    if (scratch_buffer_length == 0) {
        assert(output_buffer_length >= FFT_OUTPUT_LENGTH);

        fft_input = &output_buffer[0];
        fft_output = &output_buffer[0];
    }
    else
    {
        assert(scratch_buffer_length >= FFT_OUTPUT_LENGTH);
        assert(output_buffer_length >= RFFT_OUTPUT_LENGTH);

        fft_input = &scratch_buffer[0];
        fft_output = &scratch_buffer[0];
    }

    /// Prepare STFT input
    for (uint32_t fft_iterator = 0; fft_iterator < window_function_length; fft_iterator++) {
        assert(&input_samples_array[fft_iterator] != NULL);
        float audio_sample_float = (float)input_samples_array[fft_iterator];

        // Multiply with window function
        assert(&window_function[fft_iterator] != NULL);
        audio_sample_float = audio_sample_float * window_function[fft_iterator];

        // assert(audio_sample_float <= 1.0f && audio_sample_float >= -1.0f);
        assert(&fft_input[fft_iterator] != NULL);
        fft_input[fft_iterator] = audio_sample_float;
    }

    /// Perform real FFT
    perform_audio_dsp_rfft(
        &power_spectrum_rfft,
        fft_input,
        FFT_INPUT_LENGTH,
        fft_output,
        FFT_OUTPUT_LENGTH);

    bool
	power_threshold_reached = false;

    /// Get decibels from real FFT (discard second half of fft output)
    for (uint32_t fft_iterator = 0; fft_iterator < RFFT_OUTPUT_LENGTH; fft_iterator++) {
        assert(
            &fft_output[fft_iterator] != NULL &&
            &output_buffer[fft_iterator] != NULL);

        output_buffer[fft_iterator] = compute_decibels_from_complex(
			fft_output[fft_iterator * 2 + 0],
			fft_output[fft_iterator * 2 + 1],
            POWER_SPECTRUM_LOG_SCALE_FACTOR);
        if (
            !power_threshold_reached &&
            output_buffer[fft_iterator] >= power_threshold) {
            power_threshold_reached = true;
        }
    }

    return power_threshold_reached;
}

void
compute_power_spectrum_audio_samples_direct(
    const audio_data_type* input_samples_array,
    const uint16_t n_fft,
    float* output_buffer,
    const uint32_t output_buffer_length,
    const float* window_function,
    const uint32_t window_function_length) {
    /// Check parameters
    assert(n_fft > 0 && n_fft == window_function_length);
    assert(output_buffer_length == n_fft * 2);
    assert(input_samples_array != NULL && output_buffer != NULL);

    /// Initialise power spectrum
    initialise_power_spectrum(n_fft);
    assert(n_fft_set == n_fft);

    compute_power_spectrum_audio_samples(
        input_samples_array,
        n_fft,
        output_buffer,
        output_buffer_length,
        NULL,
        0,
        0.0f,
        window_function,
        window_function_length
    );

    /// Deinitialise power spectrum
    deinit_power_spectrum();
    assert(n_fft_set == 0);
}

