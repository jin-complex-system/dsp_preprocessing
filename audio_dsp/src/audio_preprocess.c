#include "audio_preprocess.h"

#include <stddef.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include <power_spectrum.h>
#include <mel_spectrogram.h>
#include <power_to_decibel.h>

void
audio_preprocess_compute(
    const int16_t* audio_input_buffer,
    const uint32_t audio_input_buffer_length,
    const uint32_t num_valid_audio_elements,
    float* power_spectrum_buffer,
    const uint32_t power_spectrum_buffer_length,
    float* mel_spectrogram_buffer,
    const uint32_t mel_spectrogram_buffer_length,
    uint8_t* output_buffer,
    const uint32_t output_buffer_length,
    const struct audio_preprocess_parameters parameters) {

    /// Check parameters
    {
        assert(parameters.n_fft > 0 && parameters.n_fft % 2 == 0);
        assert(parameters.hop_length > 0 && parameters.hop_length % 2 == 0);
        assert(parameters.n_fft >= parameters.hop_length);

        assert(parameters.max_frequency == 0 || parameters.max_frequency <= parameters.sample_rate);

        // TODO: Add more checks
    }

    const float*
    pHannWindow = NULL;

    switch(parameters.n_fft) {
    case 2048:
        assert(parameters.n_fft == HANN_WINDOW_SCALE_2048_BUFFER_LENGTH);
        pHannWindow = HANN_WINDOW_SCALE_2048_BUFFER;
        break;
    case 1024:
        assert(parameters.n_fft == HANN_WINDOW_SCALE_1024_BUFFER_LENGTH);
        pHannWindow = HANN_WINDOW_SCALE_1024_BUFFER;
        break;
    default:
        pHannWindow = NULL;
        assert(false);
        break;
    }
    assert(pHannWindow != NULL);

    initialise_power_spectrum(parameters.n_fft);

    /// Check parameters
    {
        assert(audio_input_buffer != NULL);
        assert(power_spectrum_buffer != NULL);
        assert(mel_spectrogram_buffer != NULL);

        assert(num_valid_audio_elements <= parameters.sample_rate * parameters.num_seconds);
        assert(mel_spectrogram_buffer_length >= parameters.num_cropped_frames);
        // TODO: Add more checks
    }

    /// Constants derived from pParametersSet and parameters
    const uint32_t AUDIO_FRAME_LENGTH = parameters.n_fft;
    const uint32_t WINDOW_LENGTH = parameters.n_fft;

    uint32_t num_valid_frames = (
        num_valid_audio_elements / parameters.hop_length -
        parameters.n_fft / parameters.hop_length) + 1;
    if (num_valid_frames > parameters.num_cropped_frames) {
        num_valid_frames =parameters.num_cropped_frames;
    }

    // TODO: Add more checks

    float* shifted_buffer = NULL;
    uint32_t num_frames_process = 0u;
    float max_mel = 1e-16f;
    uint32_t left_padding_length = 0;

    /// Will need to pad
    if (num_valid_frames < parameters.num_cropped_frames) {
        num_frames_process = num_valid_frames;
        if (parameters.left_padding) {
            left_padding_length = (parameters.num_cropped_frames - num_frames_process) / 2;
        }
        shifted_buffer = mel_spectrogram_buffer + left_padding_length;
    }
    /// Will need to crop so process less frames
    else if (num_valid_frames >= parameters.num_cropped_frames) {
        num_frames_process = parameters.num_cropped_frames;
        shifted_buffer = mel_spectrogram_buffer;
    }
    else {
        assert(false);
    }
    const uint32_t shifted_buffer_length = parameters.n_mels * num_frames_process;
    const uint32_t right_padding_length = left_padding_length;

    assert(num_frames_process != 0);
    assert(shifted_buffer != NULL);

    /// Clear buffers
    memset((void*)audio_input_buffer, 0, sizeof(audio_input_buffer));
    memset(power_spectrum_buffer, 0, sizeof(power_spectrum_buffer));
    memset(mel_spectrogram_buffer, 0, sizeof(mel_spectrogram_buffer));
    memset(output_buffer, 0, sizeof(output_buffer));

    /// Compute mel spectrogram
    for (uint32_t frame_iterator = 0; frame_iterator < num_frames_process; frame_iterator++) {
        const uint32_t audio_iterator = frame_iterator * parameters.hop_length;
        const uint32_t mel_iterator = frame_iterator * parameters.n_mels;

        compute_power_spectrum_audio_samples(
            &audio_input_buffer[audio_iterator],
            AUDIO_FRAME_LENGTH,
            &power_spectrum_buffer[0],
            power_spectrum_buffer_length,
            NULL,
            0u,
            pHannWindow,
            WINDOW_LENGTH
        );

        const float temp_max = compute_power_spectrum_into_mel_spectrogram(
            &power_spectrum_buffer[0],
            power_spectrum_buffer_length,
            &mel_spectrogram_buffer[mel_iterator],
            parameters.n_fft,
            parameters.sample_rate,
            parameters.max_frequency,
            parameters.n_mels
        );

        if (temp_max > max_mel) {
            max_mel = temp_max;
        }
    }

    /// Convert mel spectrogram to decibel and scale
    convert_power_to_decibel_and_scale(
        shifted_buffer,
        (uint8_t*)shifted_buffer,
        shifted_buffer_length,
        max_mel);

    /// If needed, clear the right side of shifted_buffer
    if (right_padding_length > 0) {
        memset(
        shifted_buffer + shifted_buffer_length,
        0,
        right_padding_length * sizeof(uint8_t));
    }

    // deinit_power_spectrum();
}

