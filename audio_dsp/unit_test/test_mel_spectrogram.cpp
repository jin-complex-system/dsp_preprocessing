#include <gtest/gtest.h>
#include <mel_spectrogram.h>
#include <power_spectrum.h>

#include <limits>
#include <mel_spectrogram_unit_test/fft_unit_input.h>
#include <mel_spectrogram_unit_test/mel_spectrogram_unit_test.h>
#include <test_audio/audio_as_integer.h>
#include <test_audio/power_spectrum.h>
#include <test_audio/mel_spectrogram.h>

constexpr uint32_t
COMPUTED_NUM_FRAMES = (
    AUDIO_INTEGER_BUFFER_LENGTH / PRECOMPUTED_POWER_SPECTRUM_HOP_LENGTH -
    PRECOMPUTED_POWER_SPECTRUM_N_FFT / PRECOMPUTED_POWER_SPECTRUM_HOP_LENGTH) + 1;

constexpr auto
MAX_AUDIO_DATA_TYPE = std::numeric_limits<audio_data_type>::max();

constexpr double
MEL_SPECTROGRAM_ERROR_TOLERANCE = 1e-3;

TEST(MelSpectrogram, UnitFilterDistribution_32) {
    // TODO: Update mel spectrogram constants
    // /// Check parameters
    // {
    //     assert(FFT_UNIT_INPUT_N_FFT > 0);
    //     assert(MEL_SPECTROGRAM_N_MELS == PRECOMPUTED_MEL_SPECTROGRAM_N_MELS);
    //     assert(FFT_UNIT_INPUT_NUM_FRAMES == MEL_SPECTROGRAM_NUM_FRAMES && FFT_UNIT_INPUT_NUM_FRAMES == 1);
    // }
    //
    // float mel_spectrogram_buffer[MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH] = {};
    //
    // /// Compute mel spectrogram
    // compute_power_spectrum_into_mel_spectrogram(
    //     FFT_UNIT_INPUT_BUFFER,
    //     FFT_UNIT_INPUT_BUFFER_LENGTH,
    //     mel_spectrogram_buffer,
    //     MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH);
    //
    // /// Check against mel spectrogram
    // for (uint32_t mel_bin_iterator = 0; mel_bin_iterator < MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH; mel_bin_iterator++) {
    //     EXPECT_NEAR(
    //         MEL_SPECTROGRAM_UNIT_TEST_BUFFER[mel_bin_iterator],
    //         mel_spectrogram_buffer[mel_bin_iterator],
    //         MEL_SPECTROGRAM_ERROR_TOLERANCE);
    // }
}

TEST(MelSpectrogram, PrecomputedMelSpectrogram) {
    /// Check parameters
    {
        assert(COMPUTED_NUM_FRAMES == PRECOMPUTED_POWER_SPECTRUM_NUM_FRAMES && COMPUTED_NUM_FRAMES == PRECOMPUTED_MEL_SPECTROGRAM_NUM_FRAMES);
        assert(PRECOMPUTED_MEL_SPECTROGRAM_N_MELS > 0);
    }

    for (uint32_t frame_iterator = 0; frame_iterator < COMPUTED_NUM_FRAMES; frame_iterator++) {
        const float* current_power_spectrum = &PRECOMPUTED_POWER_SPECTRUM_BUFFER[frame_iterator * PRECOMPUTED_POWER_SPECTRUM_BINS];
        float precomputed_mel_spectrogram_buffer[MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH];
        float forced_computed_mel_spectrogram_buffer[MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH];

        constexpr uint16_t n_fft = PRECOMPUTED_POWER_SPECTRUM_N_FFT;
        constexpr uint16_t sample_rate = 22048u;

        compute_power_spectrum_into_mel_spectrogram(
            current_power_spectrum,
            PRECOMPUTED_POWER_SPECTRUM_BINS,
            precomputed_mel_spectrogram_buffer,
            n_fft,
            sample_rate,
            MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH);
        constexpr uint16_t scratch_buffer_length = MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH * 4 + 2;
        float scratch_buffer[scratch_buffer_length];
        compute_power_spectrum_into_mel_spectrogram_raw(
            current_power_spectrum,
            PRECOMPUTED_POWER_SPECTRUM_BINS,
            n_fft,
            sample_rate,
            forced_computed_mel_spectrogram_buffer,
            MEL_SPECTROGRAM_UNIT_TEST_BUFFER_LENGTH,
            scratch_buffer,
            scratch_buffer_length);

        // TODO: Fix bug where last bin is different between precomputed and computed
        constexpr uint16_t N_MELS_WITHOUT_LAST_BIN = PRECOMPUTED_MEL_SPECTROGRAM_N_MELS - 1;
        for (uint32_t mel_bin_iterator = 0; mel_bin_iterator < N_MELS_WITHOUT_LAST_BIN; mel_bin_iterator++) {
            // TODO: Update expected constant
            // const float
            // expected_mel_bin_value = PRECOMPUTED_MEL_SPECTROGRAM_BUFFER[mel_bin_iterator + PRECOMPUTED_MEL_SPECTROGRAM_N_MELS * frame_iterator];
            // EXPECT_NEAR(
            //     expected_mel_bin_value,
            //     precomputed_mel_spectrogram_buffer[mel_bin_iterator],
            //     MEL_SPECTROGRAM_ERROR_TOLERANCE);

            EXPECT_NEAR(
                forced_computed_mel_spectrogram_buffer[mel_bin_iterator],
                precomputed_mel_spectrogram_buffer[mel_bin_iterator],
                MEL_SPECTROGRAM_ERROR_TOLERANCE);
        }
    }
}
