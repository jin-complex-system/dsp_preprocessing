#include <gtest/gtest.h>

#include <audio_dsp_fft.h>
#include <FFT_audio/rfft_input.h>
#include <FFT_audio/rfft_output.h>
#include <FFT_mels/rfft_input.h>
#include <FFT_mels/rfft_output.h>

constexpr double
FFT_AUDIO_FLOAT_ERROR_TOLERANCE = 5.0f * 1e-6;

constexpr double
FFT_MELS_FLOAT_ERROR_TOLERANCE = 5.0f * 1e-4;

TEST(FFT, FFTAudio) {
    /// Check parameters
    {
        assert(FFT_AUDIO_RFFT_INPUT_BUFFER_LENGTH % 2 == 0 && FFT_AUDIO_RFFT_INPUT_BUFFER_LENGTH > 1);
        assert(FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH == FFT_AUDIO_RFFT_INPUT_BUFFER_LENGTH + 2);
    }
    constexpr uint32_t n_fft = FFT_AUDIO_RFFT_INPUT_BUFFER_LENGTH;
    constexpr uint32_t output_buffer_length = (n_fft) * 2;
    assert(output_buffer_length >= FFT_AUDIO_RFFT_OUTPUT_BUFFER_LENGTH);
    constexpr uint32_t VALID_REAL_OUTPUT_LENGTH = n_fft / 2 + 1;
    assert(VALID_REAL_OUTPUT_LENGTH <= VALID_REAL_OUTPUT_LENGTH);

    /// Manual initialisation
    {
        audio_dsp_rfft_instance rfft_instance;
        initialise_audio_dsp_rfft(
            &rfft_instance,
            n_fft);
        EXPECT_NE(rfft_instance, nullptr);

        /// Prepare FFT output
        float output_buffer[output_buffer_length] = {};

        perform_audio_dsp_rfft(
            &rfft_instance,
            (float*)FFT_AUDIO_RFFT_INPUT_BUFFER,
            n_fft,
            output_buffer,
            output_buffer_length);

        /// Compare against output
        for (uint32_t output_buffer_iterator = 0; output_buffer_iterator < VALID_REAL_OUTPUT_LENGTH; output_buffer_iterator++) {
            /// Compare real values
            {
                const float &real_value = output_buffer[output_buffer_iterator * 2 + 0];
                const float &expected_real_value = FFT_AUDIO_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 0];
                EXPECT_NEAR(
                    real_value,
                    expected_real_value,
                    FFT_AUDIO_FLOAT_ERROR_TOLERANCE);
            }

            /// Compare imaginary values
            {
                const float &imaginary_value = output_buffer[output_buffer_iterator * 2 + 1];
                const float &expected_imaginary_value = FFT_AUDIO_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 1];
                EXPECT_NEAR(
                    imaginary_value,
                    expected_imaginary_value,
                    FFT_AUDIO_FLOAT_ERROR_TOLERANCE);
            }
        }

        /// Deinitialise
        deinitialise_audio_dsp_rfft(&rfft_instance);
    }

    /// Direct
    {
        /// Prepare FFT output
        float output_buffer[output_buffer_length] = {};

        perform_audio_dsp_rfft_direct(
            (float*)FFT_AUDIO_RFFT_INPUT_BUFFER,
            n_fft,
            output_buffer,
            output_buffer_length);

        /// Compare against output
        for (uint32_t output_buffer_iterator = 0; output_buffer_iterator < VALID_REAL_OUTPUT_LENGTH; output_buffer_iterator++) {
            /// Compare real values
            {
                const float &real_value = output_buffer[output_buffer_iterator * 2 + 0];
                const float &expected_real_value = FFT_AUDIO_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 0];
                EXPECT_NEAR(
                    real_value,
                    expected_real_value,
                    FFT_AUDIO_FLOAT_ERROR_TOLERANCE);
            }

            /// Compare imaginary values
            {
                const float &imaginary_value = output_buffer[output_buffer_iterator * 2 + 1];
                const float &expected_imaginary_value = FFT_AUDIO_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 1];
                EXPECT_NEAR(
                    imaginary_value,
                    expected_imaginary_value,
                    FFT_AUDIO_FLOAT_ERROR_TOLERANCE);
            }
        }
    }
}

TEST(FFT, FFTMels) {
    /// Check parameters
    {
        assert(FFT_MELS_RFFT_INPUT_BUFFER_LENGTH % 2 == 0 && FFT_MELS_RFFT_INPUT_BUFFER_LENGTH > 1);
        assert(FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH == FFT_MELS_RFFT_INPUT_BUFFER_LENGTH + 2);
    }
    constexpr uint32_t n_fft = FFT_MELS_RFFT_INPUT_BUFFER_LENGTH;
    constexpr uint32_t output_buffer_length = (n_fft) * 2;
    assert(output_buffer_length >= FFT_MELS_RFFT_OUTPUT_BUFFER_LENGTH);
    constexpr uint32_t VALID_REAL_OUTPUT_LENGTH = n_fft / 2 + 1;
    assert(VALID_REAL_OUTPUT_LENGTH <= VALID_REAL_OUTPUT_LENGTH);

    /// Manual initialisation
    {
        audio_dsp_rfft_instance rfft_instance;
        initialise_audio_dsp_rfft(
            &rfft_instance,
            n_fft);
        EXPECT_NE(rfft_instance, nullptr);

        /// Prepare FFT output
        float output_buffer[output_buffer_length] = {};

        perform_audio_dsp_rfft(
            &rfft_instance,
            (float*)FFT_MELS_RFFT_INPUT_BUFFER,
            n_fft,
            output_buffer,
            output_buffer_length);

        /// Compare against output
        for (uint32_t output_buffer_iterator = 0; output_buffer_iterator < VALID_REAL_OUTPUT_LENGTH; output_buffer_iterator++) {
            /// Compare real values
            {
                const float &real_value = output_buffer[output_buffer_iterator * 2 + 0];
                const float &expected_real_value = FFT_MELS_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 0];
                EXPECT_NEAR(
                    real_value,
                    expected_real_value,
                    FFT_MELS_FLOAT_ERROR_TOLERANCE);
            }

            /// Compare imaginary values
            {
                const float &imaginary_value = output_buffer[output_buffer_iterator * 2 + 1];
                const float &expected_imaginary_value = FFT_MELS_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 1];
                EXPECT_NEAR(
                    imaginary_value,
                    expected_imaginary_value,
                    FFT_MELS_FLOAT_ERROR_TOLERANCE);
            }
        }

        /// Deinitialise
        deinitialise_audio_dsp_rfft(&rfft_instance);
    }

    /// Direct
    {
        /// Prepare FFT output
        float output_buffer[output_buffer_length] = {};

        perform_audio_dsp_rfft_direct(
            (float*)FFT_MELS_RFFT_INPUT_BUFFER,
            n_fft,
            output_buffer,
            output_buffer_length);

        /// Compare against output
        for (uint32_t output_buffer_iterator = 0; output_buffer_iterator < VALID_REAL_OUTPUT_LENGTH; output_buffer_iterator++) {
            /// Compare real values
            {
                const float &real_value = output_buffer[output_buffer_iterator * 2 + 0];
                const float &expected_real_value = FFT_MELS_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 0];
                EXPECT_NEAR(
                    real_value,
                    expected_real_value,
                    FFT_MELS_FLOAT_ERROR_TOLERANCE);
            }

            /// Compare imaginary values
            {
                const float &imaginary_value = output_buffer[output_buffer_iterator * 2 + 1];
                const float &expected_imaginary_value = FFT_MELS_RFFT_OUTPUT_BUFFER[output_buffer_iterator * 2 + 1];
                EXPECT_NEAR(
                    imaginary_value,
                    expected_imaginary_value,
                    FFT_MELS_FLOAT_ERROR_TOLERANCE);
            }
        }
    }
}
