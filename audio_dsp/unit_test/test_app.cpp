#include <gtest/gtest.h>

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include <parameters.h>
#include <app_preprocess.h>

#include <precomputed_window/hann_window/hann_window_scale_2048.h>
#include <audio_dsp_fft.h>
#include <power_spectrum.h>
#include <mel_spectrogram.h>
#include <power_to_decibel.h>

#include <audio/us8k_24074_1_0_2.h>

#define INPUT_BUFFER_LENGTH AUDIO_BUFFER_MINIMUM_LENGTH
static
audio_data_type
audio_input_buffer[INPUT_BUFFER_LENGTH] __attribute__((aligned(16)));

static
float
power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH] __attribute__((aligned(16)));

static
float
mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));

static
uint8_t
output_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH] __attribute__((aligned(16)));
TEST(AudioDSPApp, CheckPowerSpectrum) {
    initialise_power_spectrum(N_FFT);

    /// Clear buffers
    memset(audio_input_buffer, 0, sizeof(audio_input_buffer));
    memset(output_buffer, 0, sizeof(output_buffer));

    memcpy(audio_input_buffer, US8K_24074_1_0_2_BUFFER, sizeof(US8K_24074_1_0_2_BUFFER));

    const uint32_t num_valid_audio_elements = US8K_24074_1_0_2_BUFFER_LENGTH;
    uint32_t num_frames_to_read =
    (num_valid_audio_elements / HOP_LENGTH - N_FFT / HOP_LENGTH) + 1;

    /// Upper limit of number of frames
    if (num_frames_to_read > NUM_FRAMES) {
        num_frames_to_read = NUM_FRAMES;
    }

    const uint32_t num_frames_comparison = 1;
    /// Test function
    {
        preprocess(
            audio_input_buffer,
            INPUT_BUFFER_LENGTH,
            power_spectrum_buffer,
            POWER_SPECTRUM_BUFFER_LENGTH,
            mel_spectrogram_buffer,
            MEL_SPECTROGRAM_BUFFER_LENGTH,
            NUM_PREPROCESS_ITERATIONS,
            num_frames_comparison,
            output_buffer,
            MEL_SPECTROGRAM_BUFFER_LENGTH
        );
    }

    audio_data_type
    another_audio_buffer[INPUT_BUFFER_LENGTH];

    float
    another_power_spectrum_buffer[POWER_SPECTRUM_BUFFER_LENGTH];

    float
    another_mel_spectrogram_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];

    uint8_t
    another_output_buffer[MEL_SPECTROGRAM_BUFFER_LENGTH];

    /// Compute power spectrum
    {
        memcpy(
            another_audio_buffer,
            US8K_24074_1_0_2_BUFFER,
            sizeof(US8K_24074_1_0_2_BUFFER));

        float max_mel = 1e-16f;
        for (uint32_t frame_iterator = 0; frame_iterator < num_frames_comparison; frame_iterator++) {
            const uint32_t audio_iterator = frame_iterator * HOP_LENGTH;
            const uint32_t power_spectrum_iterator = 0;
            const uint32_t mel_iterator = frame_iterator * N_MELS;

            compute_power_spectrum_audio_samples(
                &another_audio_buffer[audio_iterator],
                AUDIO_FRAME_LENGTH,
                &another_power_spectrum_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_BUFFER_LENGTH,
                NULL,
                0u,
                HANN_WINDOW_SCALE_2048_BUFFER,
                HANN_WINDOW_SCALE_2048_BUFFER_LENGTH
            );

            const float temp_max = compute_power_spectrum_into_mel_spectrogram(
                &another_power_spectrum_buffer[power_spectrum_iterator],
                POWER_SPECTRUM_LENGTH,
                &another_mel_spectrogram_buffer[mel_iterator],
                N_FFT,
                SAMPLING_RATE_PER_SECOND,
                MAX_FREQUENCY,
                N_MELS
            );

            if (temp_max > max_mel) {
                max_mel = temp_max;
            }
        }

        convert_power_to_decibel_and_scale(
            another_mel_spectrogram_buffer,
            another_output_buffer,
            N_MELS * num_frames_comparison,
            max_mel);
    }

    /// Compare buffers in power spectrum
    {
        const int power_spectrum_memcmp_result =
            memcmp(
                another_power_spectrum_buffer,
                power_spectrum_buffer,
                num_frames_comparison * POWER_SPECTRUM_LENGTH * sizeof(float));
        EXPECT_EQ(power_spectrum_memcmp_result, 0);

        EXPECT_EQ(0, 1);
        std::cout << power_spectrum_buffer[0] << std::endl;
        std::cout << power_spectrum_buffer[1] << std::endl;
        std::cout << power_spectrum_buffer[2] << std::endl;
        std::cout << power_spectrum_buffer[3] << std::endl;

        /// Locate different values
        if (power_spectrum_memcmp_result != 0) {
            std::cout << "Power spectrum buffer mismatch!" << std::endl;
            for (uint32_t frame_iterator = 0; frame_iterator < num_frames_comparison; frame_iterator++) {
                for (uint32_t fft_bin_iterator = 0; fft_bin_iterator < POWER_SPECTRUM_LENGTH; fft_bin_iterator++) {
                    const uint32_t power_spectrum_iterator = fft_bin_iterator + frame_iterator * POWER_SPECTRUM_LENGTH;

                    if (
                        another_power_spectrum_buffer[power_spectrum_iterator] !=
                        power_spectrum_buffer[power_spectrum_iterator]) {
                        std::cout <<"At fft bin " << fft_bin_iterator << " and frame " << frame_iterator <<", mismatch!" << std::endl;
                        EXPECT_FLOAT_EQ(
                            another_power_spectrum_buffer[power_spectrum_iterator],
                            power_spectrum_buffer[power_spectrum_iterator]);
                        }
                }
            }
        }
    }

    /// Compare buffers in mel spectrogram buffer
    {
        const int mel_spec_memcmp_result =
            memcmp(
                another_mel_spectrogram_buffer,
                mel_spectrogram_buffer,
                num_frames_comparison * N_MELS * sizeof(float));
        EXPECT_EQ(mel_spec_memcmp_result, 0);

        /// Locate different values
        if (mel_spec_memcmp_result != 0) {
            std::cout << "Mel spectrogram buffer mismatch!" << std::endl;
            for (uint32_t frame_iterator = 0; frame_iterator < num_frames_comparison; frame_iterator++) {
                for (uint32_t mel_bin_iterator = 0; mel_bin_iterator < N_MELS; mel_bin_iterator++) {
                    const uint32_t mel_iterator = frame_iterator * N_MELS + mel_bin_iterator;

                    if (
                        another_mel_spectrogram_buffer[mel_iterator] !=
                        mel_spectrogram_buffer[mel_iterator]) {
                        std::cout <<"At mel " << mel_bin_iterator << " and frame " << frame_iterator <<", mismatch!" << std::endl;
                        EXPECT_FLOAT_EQ(
                            another_mel_spectrogram_buffer[mel_iterator],
                            mel_spectrogram_buffer[mel_iterator]);
                        }
                }
            }
        }
    }

    /// Compare buffers in output buffer
    {
        const int output_buffer_memcmp_result =
            memcmp(
                another_output_buffer,
                output_buffer,
                num_frames_comparison * N_MELS * sizeof(float));
        EXPECT_EQ(output_buffer_memcmp_result, 0);

        /// Locate different values
        if (output_buffer_memcmp_result != 0) {
            std::cout << "Output buffer mismatch!" << std::endl;
            for (uint32_t frame_iterator = 0; frame_iterator < num_frames_comparison; frame_iterator++) {
                for (uint32_t mel_bin_iterator = 0; mel_bin_iterator < N_MELS; mel_bin_iterator++) {
                    const uint32_t mel_iterator = frame_iterator * N_MELS + mel_bin_iterator;

                    if (
                        another_output_buffer[mel_iterator] !=
                        output_buffer[mel_iterator]) {
                        std::cout <<"At mel " << mel_bin_iterator << " and frame " << frame_iterator <<", mismatch!" << std::endl;
                        EXPECT_EQ(
                            another_output_buffer[mel_iterator],
                            output_buffer[mel_iterator]);
                        }
                }
            }
        }
    }
}
