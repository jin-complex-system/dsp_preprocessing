#include <gtest/gtest.h>

#include <audio_preprocess.h>

#include <cmath>
#include <cassert>
#include <cstring>

/// Constants
#include <audio/human_voice.h>

/// Audio DSP
#include <precomputed_window/hann_window/hann_window_scale_2048.h>
#include <hann_window_compute.h>
#include <audio_dsp_fft.h>
#include <power_spectrum.h>
#include <mel_spectrogram.h>
#include <power_to_decibel.h>

TEST(AudioPreprocess, ExampleHumanVoiceCropping) {
    /// Define basic constants
    constexpr uint32_t SAMPLING_RATE_PER_SECOND = 44100;
    assert(SAMPLING_RATE_PER_SECOND == HUMAN_VOICE_SAMPLE_RATE);
    constexpr uint32_t DURATION_SECONDS = HUMAN_VOICE_DURATION;
    constexpr uint32_t N_FFT = 2048;
    constexpr uint32_t HOP_LENGTH = 512;
    constexpr uint32_t N_MELS = 64;
    constexpr uint32_t MAX_FREQUENCY = 8000u;
    constexpr float TOP_DECIBEL = 80.0f;
    constexpr uint32_t NUM_VALID_AUDIO_ELEMENTS = HUMAN_VOICE_BUFFER_LENGTH;

    /// Intermediate constants, based on above constants
    constexpr uint32_t AUDIO_FRAME_LENGTH = N_FFT;
    constexpr uint32_t AUDIO_INPUT_BUFFER_LENGTH = SAMPLING_RATE_PER_SECOND * DURATION_SECONDS;
    constexpr uint32_t NUM_FRAMES = ((AUDIO_INPUT_BUFFER_LENGTH / HOP_LENGTH - N_FFT/HOP_LENGTH) + 1);
    constexpr uint32_t REAL_FREQUENCY_BIN_LENGTHS = N_FFT / 2 + 1;
    constexpr uint32_t POWER_SPECTRUM_LENGTH = REAL_FREQUENCY_BIN_LENGTHS;

    /// Buffer lengths
    constexpr uint32_t POWER_SPECTRUM_BUFFER_LENGTH = N_FFT * 2;
    constexpr uint32_t MEL_SPECTROGRAM_BUFFER_LENGTH = N_MELS * NUM_FRAMES;
    constexpr uint32_t OUTPUT_BUFFER_LENGTH = MEL_SPECTROGRAM_BUFFER_LENGTH;

    /// Define test buffers
    int16_t
    audio_input_buffer[AUDIO_INPUT_BUFFER_LENGTH];
    float
    power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH];
    float
    mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];
    uint8_t
    output_buffer[OUTPUT_BUFFER_LENGTH];

    /*
    /// Compute
    {
        // Intentional if we can't copy the entire buffer
        memcpy((void*)audio_input_buffer, (void*)HUMAN_VOICE_BUFFER, sizeof(audio_input_buffer));

        constexpr audio_preprocess_parameters parameters = {
            .n_fft = N_FFT,
            .hop_length = HOP_LENGTH,
            .n_mels = N_MELS,
            .sample_rate = SAMPLING_RATE_PER_SECOND,
            .num_seconds = 7u,
            .max_frequency = MAX_FREQUENCY,

            .num_cropped_frames = 384u,
            .left_padding = false,
        };
        audio_preprocess_compute(
            audio_input_buffer,
            AUDIO_INPUT_BUFFER_LENGTH,
            NUM_VALID_AUDIO_ELEMENTS,
            power_spectrum_buffer,
            POWER_SPECTRUM_BUFFER_LENGTH,
            mel_spectrogram_buffer,
            MEL_SPECTROGRAM_BUFFER_LENGTH,
            output_buffer,
            OUTPUT_BUFFER_LENGTH,
            parameters);
    }
    */

    /// TODO: Compare expected results
    {
    }
}

