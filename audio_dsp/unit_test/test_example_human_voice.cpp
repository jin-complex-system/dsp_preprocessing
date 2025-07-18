#include <gtest/gtest.h>

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

/// Scaler
#include <scaler_float.h>

TEST(AudioDSP, ExampleHumanVoice) {
    /// Define basic constants
    constexpr uint32_t SAMPLING_RATE_PER_SECOND = 44100;
    assert(SAMPLING_RATE_PER_SECOND == HUMAN_VOICE_SAMPLE_RATE);
    constexpr uint32_t DURATION_SECONDS = HUMAN_VOICE_DURATION;
    constexpr uint32_t N_FFT = 2048;
    constexpr uint32_t HOP_LENGTH = 512;
    constexpr uint32_t N_MELS = 64;
    constexpr uint32_t MAX_FREQUENCY = 8000u;
    constexpr float TOP_DECIBEL = 80.0f;

    /// Intermediate constants, based on above constants
    constexpr uint32_t AUDIO_FRAME_LENGTH = N_FFT;
    constexpr uint32_t AUDIO_INPUT_BUFFER_LENGTH = SAMPLING_RATE_PER_SECOND * DURATION_SECONDS;
    constexpr uint32_t NUM_FRAMES = ((AUDIO_INPUT_BUFFER_LENGTH / HOP_LENGTH - N_FFT/HOP_LENGTH) + 1);
    constexpr uint32_t REAL_FREQUENCY_BIN_LENGTHS = N_FFT / 2 + 1;
    constexpr uint32_t POWER_SPECTRUM_LENGTH = REAL_FREQUENCY_BIN_LENGTHS;

    /// Buffer lengths
    constexpr uint32_t POWER_SPECTRUM_BUFFER_LENGTH = N_FFT * 2;
    constexpr uint32_t MEL_SPECTROGRAM_BUFFER_LENGTH = N_MELS * NUM_FRAMES;

    /// Define test buffers
    int16_t
    audio_input_buffer[AUDIO_INPUT_BUFFER_LENGTH];
    float
    power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH];
    float
    mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];

    /// Check precomputation values
    {
        float hann_window_buffer_2048[N_FFT];

        constexpr float
        scaling_factor = 1.0 / INT16_MAX;

        hann_window_compute(
            hann_window_buffer_2048,
            N_FFT,
            scaling_factor);

        const auto memcmp_result = memcmp(
            (void*)hann_window_buffer_2048,
            (void*)HANN_WINDOW_SCALE_2048_BUFFER,
            sizeof(hann_window_buffer_2048));
        EXPECT_EQ(memcmp_result, 0);
    }

    /// Initialise power spectrum
    initialise_power_spectrum(N_FFT);

    constexpr float INITIAL_MAX_MEL = 1e-16f;
    /// Begin computation of human voice signal
    {
        // Intentional if we can't copy the entire buffer
        memcpy(audio_input_buffer, HUMAN_VOICE_BUFFER, sizeof(audio_input_buffer));

        float max_mel = INITIAL_MAX_MEL;

        /// Compute power spectrum
        for (uint32_t frame_iterator = 0; frame_iterator < NUM_FRAMES; frame_iterator++) {
            const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;

            compute_power_spectrum_audio_samples(
                &audio_input_buffer[audio_iterator],
                AUDIO_FRAME_LENGTH,
                power_spectrum_buffer,
                POWER_SPECTRUM_BUFFER_LENGTH,
                nullptr,
                0u,
                HANN_WINDOW_SCALE_2048_BUFFER,
                HANN_WINDOW_SCALE_2048_BUFFER_LENGTH
            );

            /// Check power spectrum buffer
            {
                int16_t expected_buffer[AUDIO_FRAME_LENGTH];
                memcpy(expected_buffer, &HUMAN_VOICE_BUFFER[audio_iterator], sizeof(expected_buffer));

                float expected_power_spectrum[POWER_SPECTRUM_LENGTH];
                constexpr uint32_t scratch_buffer_length = AUDIO_FRAME_LENGTH * 2;
                float scratch_buffer[scratch_buffer_length];

                compute_power_spectrum_audio_samples(
                    expected_buffer,
                    AUDIO_FRAME_LENGTH,
                    expected_power_spectrum,
                    POWER_SPECTRUM_BUFFER_LENGTH,
                    scratch_buffer,
                    scratch_buffer_length,
                    HANN_WINDOW_SCALE_2048_BUFFER,
                    HANN_WINDOW_SCALE_2048_BUFFER_LENGTH
                );

                const auto memcmp_result = memcmp(
                    (void*)expected_power_spectrum,
                    (void*)power_spectrum_buffer,
                    sizeof(expected_power_spectrum));
                EXPECT_EQ(memcmp_result, 0);
            }

            const float temp_max = compute_power_spectrum_into_mel_spectrogram(
                &power_spectrum_buffer[0],
                POWER_SPECTRUM_LENGTH,
                &mel_spectrogram_buffer[frame_iterator * N_MELS],
                N_FFT,
                SAMPLING_RATE_PER_SECOND,
                MAX_FREQUENCY,
                N_MELS
            );

            /// Check mel spectrogram buffer against raw computation
            {
                float expected_mel_spectrogram[N_MELS];

                constexpr uint32_t SCRATCH_BUFFER_LENGTH = N_MELS * 4 + 2;
                float scratch_buffer[N_MELS * 4 + 2];

                const float expected_max = compute_power_spectrum_into_mel_spectrogram_raw(
                    &power_spectrum_buffer[0],
                    POWER_SPECTRUM_LENGTH,
                    N_FFT,
                    SAMPLING_RATE_PER_SECOND,
                    MAX_FREQUENCY,
                    expected_mel_spectrogram,
                    N_MELS,
                    scratch_buffer,
                    SCRATCH_BUFFER_LENGTH
                );
                EXPECT_FLOAT_EQ(temp_max, expected_max);

                const auto memcmp_result = memcmp(
                    (void*)expected_mel_spectrogram,
                    (void*)&mel_spectrogram_buffer[frame_iterator * N_MELS],
                    sizeof(expected_mel_spectrogram));
                EXPECT_EQ(memcmp_result, 0);

                /// Locate different values
                if (memcmp_result != 0) {
                    for (uint32_t mel_iterator = 0; mel_iterator < N_MELS; mel_iterator++) {
                        if (
                            expected_mel_spectrogram[mel_iterator] !=
                            mel_spectrogram_buffer[frame_iterator * N_MELS + mel_iterator]) {
                            std::cout <<"At mel " << mel_iterator << ", mismatch!" << std::endl;
                            EXPECT_FLOAT_EQ(
                                expected_mel_spectrogram[mel_iterator],
                                mel_spectrogram_buffer[frame_iterator * N_MELS + mel_iterator]);
                        }
                    }
                }
            }

            if (temp_max > max_mel) {
                max_mel = temp_max;
            }
        }
        EXPECT_GE(max_mel, INITIAL_MAX_MEL);

        float mel_before_scaling_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];
        memcpy(mel_before_scaling_buffer, mel_spectrogram_buffer, sizeof(mel_before_scaling_buffer));

        convert_power_to_decibel_and_scale(
            mel_spectrogram_buffer,
            (uint8_t*)mel_spectrogram_buffer,
            MEL_SPECTROGRAM_BUFFER_LENGTH,
            max_mel);

        /// Save result into a buffer from copying
        uint8_t computed_mel_scale_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];
        memcpy(computed_mel_scale_buffer, (uint8_t*)mel_spectrogram_buffer, MEL_SPECTROGRAM_BUFFER_LENGTH);

        //// Scale independently
        {
            float mel_input_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];
            memcpy(mel_input_buffer, mel_before_scaling_buffer, sizeof(mel_before_scaling_buffer));

            convert_power_to_decibel(
                mel_input_buffer,
                MEL_SPECTROGRAM_BUFFER_LENGTH,
                max_mel,
                TOP_DECIBEL
            );

            uint8_t expected_mel_scale_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];
            scale_float_buffer_quantised(
                mel_input_buffer,
                expected_mel_scale_buffer,
                MEL_SPECTROGRAM_BUFFER_LENGTH,
                _power_to_decibel_get_max_decibel(),
                _power_to_decibel_get_min_decibel()
            );

            /// Compare copied buffer to expected
            {
                const auto memcmp_result = memcmp(
                   (void*)expected_mel_scale_buffer,
                   (void*)computed_mel_scale_buffer,
                   MEL_SPECTROGRAM_BUFFER_LENGTH);
                EXPECT_EQ(memcmp_result, 0);
            }

            /// Compare mel spectrogram referenced as uint8
            {
                const auto memcmp_result = memcmp(
                    (void*)mel_spectrogram_buffer,
                    (void*)computed_mel_scale_buffer,
                    MEL_SPECTROGRAM_BUFFER_LENGTH);
                EXPECT_EQ(memcmp_result, 0);
            }

        }
    }
}
