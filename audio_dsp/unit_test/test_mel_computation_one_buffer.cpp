#include <gtest/gtest.h>
#include <mel_spectrogram.h>
#include <power_spectrum.h>

TEST(MelSpectrogram, PrecomputationOneBuffer) {
    constexpr uint16_t n_mel = 64;
    constexpr uint16_t n_fft = 2048;
    constexpr uint16_t sample_rate = 44100;
    constexpr uint16_t max_frequency = 8000u;

    constexpr uint16_t MEL_CENTRE_FREQ_FLOAT_BUFFER_LENGTH = n_mel + 2;
    constexpr uint16_t MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_LENGTH = n_mel + 0;
    constexpr uint16_t MEL_CENTRE_FREQ_PREV_BIN_BUFFER_LENGTH = n_mel + 0;
    constexpr uint16_t MEL_FREQ_WEIGHTS_BUFFER_LENGTH = n_mel + 0;

    constexpr uint16_t MEL_CENTRE_FREQ_FLOAT_BUFFER_SIZE = MEL_CENTRE_FREQ_FLOAT_BUFFER_LENGTH * sizeof(float);
    constexpr uint16_t MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_SIZE = MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_LENGTH * sizeof(uint16_t);
    constexpr uint16_t MEL_CENTRE_FREQ_PREV_BIN_BUFFER_SIZE = MEL_CENTRE_FREQ_PREV_BIN_BUFFER_LENGTH * sizeof(uint16_t);
    constexpr uint16_t MEL_FREQ_WEIGHTS_BUFFER_SIZE = MEL_FREQ_WEIGHTS_BUFFER_LENGTH * sizeof(float);

    const float* p_precomputed_mel_float = nullptr;
    const uint16_t* p_precomputed_next_bin = nullptr;
    const uint16_t* p_precomputed_prev_bin = nullptr;
    const float* p_precomputed_weights = nullptr;

    const bool get_precomputed_value = get_mel_spectrogram_precomputed_values(
        n_mel,
        n_fft,
        sample_rate,
        max_frequency,
        &p_precomputed_mel_float,
        &p_precomputed_next_bin,
        &p_precomputed_prev_bin,
        &p_precomputed_weights);
    EXPECT_EQ(true, get_precomputed_value);

    /// Compare mel spectrogram bins in one buffer
    {
        constexpr uint16_t SCRATCH_BUFFER_SIZE = MEL_CENTRE_FREQ_FLOAT_BUFFER_SIZE + MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_SIZE
        + MEL_CENTRE_FREQ_PREV_BIN_BUFFER_SIZE + MEL_FREQ_WEIGHTS_BUFFER_SIZE;
        uint8_t SCRATCH_BUFFER[SCRATCH_BUFFER_SIZE] = {};

        /// Compute iterator to SCRATCH_BUFFER
        constexpr uint16_t MEL_CENTRE_FLOAT_ITERATOR = 0;
        constexpr uint16_t NEXT_BIN_ITERATOR = MEL_CENTRE_FREQ_FLOAT_BUFFER_SIZE + MEL_CENTRE_FLOAT_ITERATOR;
        constexpr uint16_t PREV_BIN_ITERATOR = MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_SIZE + NEXT_BIN_ITERATOR;
        constexpr uint16_t WEIGHTS_ITERATOR = MEL_CENTRE_FREQ_PREV_BIN_BUFFER_SIZE + PREV_BIN_ITERATOR;

        auto mel_centre_freq_float_buffer = (float*)&SCRATCH_BUFFER[MEL_CENTRE_FLOAT_ITERATOR];
        auto mel_centre_freq_next_bin_buffer = (uint16_t*)&SCRATCH_BUFFER[NEXT_BIN_ITERATOR];
        auto mel_centre_freq_prev_bin_buffer =(uint16_t*)&SCRATCH_BUFFER[PREV_BIN_ITERATOR];
        auto mel_freq_weights_buffer = (float*)&SCRATCH_BUFFER[WEIGHTS_ITERATOR];
        assert(mel_centre_freq_float_buffer[1] == 0.0f);

        compute_mel_spectrogram_bins(
            n_mel,
            n_fft,
            sample_rate,
            max_frequency,
            mel_centre_freq_float_buffer,
            mel_centre_freq_next_bin_buffer,
            mel_centre_freq_prev_bin_buffer,
            mel_freq_weights_buffer);
        EXPECT_FLOAT_EQ(mel_centre_freq_float_buffer[0], 0.0f);
        EXPECT_NE(mel_centre_freq_float_buffer[1], 0.0f);

        /// Compare against precomputed buffer
        {
            /// Check mel centre float
            {
                const int memcmp_result = memcmp(
                    (void*)p_precomputed_mel_float,
                    (void*)mel_centre_freq_float_buffer,
                    sizeof(mel_centre_freq_float_buffer));
                EXPECT_EQ(0, memcmp_result);

                /// If not the same, find the mismatch
                if (memcmp_result != 0 ) {
                    for (uint32_t iterator = 0; iterator < MEL_CENTRE_FREQ_FLOAT_BUFFER_LENGTH; iterator++) {
                        const float &precomputed_float = p_precomputed_mel_float[iterator];
                        const float &computed_float = mel_centre_freq_float_buffer[iterator];

                        if (computed_float != precomputed_float) {
                            std::cout << "Freq float mismatch at " << iterator << std::endl;
                            EXPECT_FLOAT_EQ(precomputed_float, computed_float);
                        }
                    }
                }
            }

            /// Check next bin
            {
                const int memcmp_result = memcmp(
            (void*)p_precomputed_next_bin,
            (void*)mel_centre_freq_next_bin_buffer,
            sizeof(MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_LENGTH));
                EXPECT_EQ(0, memcmp_result);

                /// If not the same, find the mismatch
                if (memcmp_result != 0 ) {
                    for (uint32_t iterator = 0; iterator < MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_LENGTH; iterator++) {
                        const uint16_t &precomputed_next_bin = p_precomputed_next_bin[iterator];
                        const uint16_t &computed_next_bin = mel_centre_freq_next_bin_buffer[iterator];

                        if (precomputed_next_bin != computed_next_bin) {
                            std::cout << "Next bin mismatch at " << iterator << std::endl;
                            EXPECT_EQ(precomputed_next_bin, computed_next_bin);
                        }
                    }
                }
            }

            /// Check prev bin
            {
                const int memcmp_result = memcmp(
            (void*)p_precomputed_prev_bin,
            (void*)mel_centre_freq_prev_bin_buffer,
            sizeof(MEL_CENTRE_FREQ_PREV_BIN_BUFFER_LENGTH));
                EXPECT_EQ(0, memcmp_result);

                /// If not the same, find the mismatch
                if (memcmp_result != 0 ) {
                    for (uint32_t iterator = 0; iterator < MEL_CENTRE_FREQ_PREV_BIN_BUFFER_LENGTH; iterator++) {
                        const uint16_t &precomputed_prev_bin = p_precomputed_prev_bin[iterator];
                        const uint16_t &computed_prev_bin = mel_centre_freq_prev_bin_buffer[iterator];

                        if (precomputed_prev_bin != computed_prev_bin) {
                            std::cout << "Next bin mismatch at " << iterator << std::endl;
                            EXPECT_EQ(precomputed_prev_bin, computed_prev_bin);
                        }
                    }
                }
            }

            /// Check weights
            {
                const int memcmp_result = memcmp(
                (void*)p_precomputed_weights,
                (void*)mel_freq_weights_buffer,
                sizeof(MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_LENGTH));
                EXPECT_EQ(0, memcmp_result);

                /// If not the same, find the mismatch
                if (memcmp_result != 0 ) {
                    for (uint32_t iterator = 0; iterator < MEL_CENTRE_FREQ_NEXT_BIN_BUFFER_LENGTH; iterator++) {
                        const float &precomputed_weights = p_precomputed_weights[iterator];
                        const float &computed_weights = mel_freq_weights_buffer[iterator];

                        if (precomputed_weights != computed_weights) {
                            std::cout << "Next bin mismatch at " << iterator << std::endl;
                            EXPECT_FLOAT_EQ(precomputed_weights, computed_weights);
                        }
                    }
                }

            }
        }
    }
}
