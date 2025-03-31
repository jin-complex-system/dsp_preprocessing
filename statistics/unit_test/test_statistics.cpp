#include <gtest/gtest.h>
#include <statistics.h>

#include <cstring>
#include <vector>
#include <cstdlib>

#include <mel_spectrogram.h>
#include <mel_spectrogram_stats.h>
#include <cepstrum.h>
#include <cepstrum_stats.h>
#include <combined_stats.h>

constexpr double
FLOAT_ERROR_TOLERANCE = 5 * 1e-5;

constexpr double
FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE = 0.5;

constexpr double
FIRST_DERIVATIVE_VARIANCE_ERROR_TOLERANCE = 5.0;

static
void
fill_input(
    float* input_buffer,
    const uint32_t input_buffer_length) {
    assert(input_buffer_length > 0);

    for (uint32_t iterator = 0; iterator < input_buffer_length; iterator++) {
        input_buffer[iterator] = (float)rand();
    }
}

TEST(Statistics, ComputeStatsWithinFrame) {
    constexpr uint32_t BIN_LENGTH = 2;

    /// All zeros
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr auto INPUT_LENGTH = BIN_LENGTH;
        constexpr uint32_t
        SCRATCH_LENGTH = 2 * INPUT_LENGTH;
        constexpr float input_array[INPUT_LENGTH] = {};
        float scratch_array[SCRATCH_LENGTH] = {};
        struct statistics statistics_output = {};

        compute_statistics_within_frame(
            input_array,
            BIN_LENGTH,
            FRAME_DIFFERENCE_RECIPROCAL,
            &statistics_output,
            1,
            scratch_array,
            SCRATCH_LENGTH
        );


        EXPECT_FLOAT_EQ(statistics_output.max, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.min, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.mean, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.median, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.variance, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_mean, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_variance, 0.0f);
    }

    /// Typical example
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr auto INPUT_LENGTH = 5u;
        constexpr float input_array[INPUT_LENGTH] = {
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        };
        constexpr uint32_t
        SCRATCH_LENGTH = 2 * INPUT_LENGTH;
        float scratch_array[SCRATCH_LENGTH] = {};
        struct statistics statistics_output;

        compute_statistics_within_frame(
            input_array,
            INPUT_LENGTH,
            FRAME_DIFFERENCE_RECIPROCAL,
            &statistics_output,
            1,
            scratch_array,
            SCRATCH_LENGTH
        );

        EXPECT_FLOAT_EQ(statistics_output.max, 1.0f);
        EXPECT_FLOAT_EQ(statistics_output.min, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.mean, 1.0f / INPUT_LENGTH);
        EXPECT_FLOAT_EQ(statistics_output.median, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.variance, 0.4f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_mean, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_variance, 0.70710677f);
    }
}

TEST(Statistics, ComputeStatsAcrossFrame) {
    constexpr uint32_t BIN_LENGTH = 2;
    constexpr uint32_t NUM_FRAMES = 4u;

    constexpr uint32_t
    INPUT_LENGTH = BIN_LENGTH * NUM_FRAMES;

    constexpr uint32_t
    SCRATCH_LENGTH = 2 * NUM_FRAMES;

    constexpr uint32_t
    OUTPUT_LENGTH = BIN_LENGTH;

    assert(INPUT_LENGTH > 0);

    /// General example
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr auto INPUT_LENGTH = 5u;
        constexpr float input_array[INPUT_LENGTH] = {
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        };
        constexpr uint32_t
        SCRATCH_LENGTH = 2 * INPUT_LENGTH;
        float scratch_array[SCRATCH_LENGTH] = {};
        struct statistics statistics_output[OUTPUT_LENGTH] = {};

        compute_statistics_within_frame(
            input_array,
            INPUT_LENGTH,
            FRAME_DIFFERENCE_RECIPROCAL,
            statistics_output,
            OUTPUT_LENGTH,
            scratch_array,
            SCRATCH_LENGTH
        );
        EXPECT_FLOAT_EQ(statistics_output->max, 1.0f);
        EXPECT_FLOAT_EQ(statistics_output->min, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output->mean, 1.0f / INPUT_LENGTH);
        EXPECT_FLOAT_EQ(statistics_output->median, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output->variance, 0.4f);
        // EXPECT_FLOAT_EQ(statistics_output->first_der_mean, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output->first_der_variance, 0.70710677f);

    }

    /// Use compute_statistics_within_frame() to compare results
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        float input_array[INPUT_LENGTH];

        fill_input(
            input_array,
            INPUT_LENGTH);
        float across_frames_scratch_array[SCRATCH_LENGTH] = {};
        struct statistics across_frames_statistics_array[OUTPUT_LENGTH] = {};

        compute_statistics_across_frames(
            input_array,
            NUM_FRAMES,
            BIN_LENGTH,
            FRAME_DIFFERENCE_RECIPROCAL,
            across_frames_statistics_array,
            OUTPUT_LENGTH,
            across_frames_scratch_array,
            SCRATCH_LENGTH
        );

        float within_frame_input_array[NUM_FRAMES] = {};
        for (uint32_t bin_iterator = 0; bin_iterator < BIN_LENGTH; bin_iterator++) {
            for (uint32_t frame_iterator = 0; frame_iterator < NUM_FRAMES; frame_iterator++) {
                const uint32_t input_iterator = bin_iterator + frame_iterator * BIN_LENGTH;
                assert(input_iterator < INPUT_LENGTH);

                within_frame_input_array[frame_iterator] = input_array[input_iterator];
            }

            float within_frames_scratch_array[2 * NUM_FRAMES] = {};
            struct statistics within_frames_statistics_output = {};

            compute_statistics_within_frame(
                within_frame_input_array,
                NUM_FRAMES,
                FRAME_DIFFERENCE_RECIPROCAL,
                &within_frames_statistics_output,
    1,
    within_frames_scratch_array,
    2 * NUM_FRAMES
            );

            auto across_frames_statistics_output =
                (const struct statistics*)&across_frames_statistics_array[bin_iterator];
            EXPECT_FLOAT_EQ(across_frames_statistics_output->max, within_frames_statistics_output.max);
            EXPECT_FLOAT_EQ(across_frames_statistics_output->min, within_frames_statistics_output.min);
            EXPECT_FLOAT_EQ(across_frames_statistics_output->mean, within_frames_statistics_output.mean);
            EXPECT_FLOAT_EQ(across_frames_statistics_output->median, within_frames_statistics_output.median);
            EXPECT_FLOAT_EQ(across_frames_statistics_output->variance, within_frames_statistics_output.variance);
            // EXPECT_FLOAT_EQ(across_frames_statistics_output->first_der_mean, within_frames_statistics_output.first_der_mean);
            // EXPECT_FLOAT_EQ(across_frames_statistics_output->first_der_variance, within_frames_statistics_output.first_der_variance);
        }
    }
}

TEST(Statistics, PrecomputedMelSpectrogramStats) {
    /// Check parameters
    {
        assert(PRECOMPUTED_MEL_SPECTROGRAM_STATS_NUM_STATS == STATISTICS_ELEMENT_LENGTH);
        assert(PRECOMPUTED_MEL_SPECTROGRAM_N_MELS == PRECOMPUTED_MEL_SPECTROGRAM_STATS_N_MELS);
    }
    constexpr uint32_t output_buffer_length = PRECOMPUTED_MEL_SPECTROGRAM_STATS_N_MELS;
    struct statistics statistics_output_buffer[output_buffer_length];

    constexpr uint32_t scratch_buffer_length = 2 * PRECOMPUTED_MEL_SPECTROGRAM_NUM_FRAMES * PRECOMPUTED_MEL_SPECTROGRAM_N_MELS;
    float scratch_array[scratch_buffer_length] = {};

    compute_statistics_across_frames(
        PRECOMPUTED_MEL_SPECTROGRAM_BUFFER,
        PRECOMPUTED_MEL_SPECTROGRAM_NUM_FRAMES,
        PRECOMPUTED_MEL_SPECTROGRAM_N_MELS,
        PRECOMPUTED_MEL_SPECTROGRAM_STATS_SAMPLE_RATE_RECIPROCAL,
        statistics_output_buffer,
        output_buffer_length,
        scratch_array,
        scratch_buffer_length
    );

    for (uint32_t output_iterator = 0; output_iterator < output_buffer_length; output_iterator++) {
        auto statistics_output =
            (const struct statistics*)&statistics_output_buffer[output_iterator];
        auto expected_output =
            (const struct statistics*)&PRECOMPUTED_MEL_SPECTROGRAM_STATS_BUFFER[output_iterator * STATISTICS_ELEMENT_LENGTH];

        EXPECT_FLOAT_EQ(statistics_output->max, expected_output->max);
        EXPECT_FLOAT_EQ(statistics_output->min, expected_output->min);
        EXPECT_FLOAT_EQ(statistics_output->mean, expected_output->mean);
        EXPECT_FLOAT_EQ(statistics_output->median, expected_output->median);
        EXPECT_NEAR(statistics_output->variance, expected_output->variance, FLOAT_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_variance, expected_output->first_der_variance, FIRST_DERIVATIVE_VARIANCE_ERROR_TOLERANCE);
    }
}

TEST(Statistics, PrecomputedCepstrumStats) {
    /// Check parameters
    {
        assert(PRECOMPUTED_CEPSTRUM_COEFFICIENTS_NUM_STATS == STATISTICS_ELEMENT_LENGTH);
        assert(PRECOMPUTED_CEPSTRUM_COEFFICIENTS == PRECOMPUTED_CEPSTRUM_COEFFICIENTS_STATS);
    }
    constexpr uint32_t output_buffer_length = PRECOMPUTED_CEPSTRUM_COEFFICIENTS;
    struct statistics statistics_output_buffer[output_buffer_length];

    constexpr uint32_t scratch_buffer_length = 2 * PRECOMPUTED_CEPSTRUM_NUM_FRAMES * PRECOMPUTED_CEPSTRUM_COEFFICIENTS;
    float scratch_array[scratch_buffer_length] = {};

    compute_statistics_across_frames(
        PRECOMPUTED_CEPSTRUM_BUFFER,
        PRECOMPUTED_CEPSTRUM_NUM_FRAMES,
        PRECOMPUTED_CEPSTRUM_COEFFICIENTS,
        PRECOMPUTED_CEPSTRUM_COEFFICIENTS_STATS_SAMPLE_RATE_RECIPROCAL,
        statistics_output_buffer,
        output_buffer_length,
        scratch_array,
        scratch_buffer_length
    );

    for (uint32_t output_iterator = 0; output_iterator < output_buffer_length; output_iterator++) {
        auto statistics_output =
            (const struct statistics*)&statistics_output_buffer[output_iterator];
        auto expected_output =
            (const struct statistics*)&PRECOMPUTED_CEPSTRUM_STATS_BUFFER[output_iterator * STATISTICS_ELEMENT_LENGTH];

        EXPECT_FLOAT_EQ(statistics_output->max, expected_output->max);
        EXPECT_FLOAT_EQ(statistics_output->min, expected_output->min);
        EXPECT_FLOAT_EQ(statistics_output->mean, expected_output->mean);
        EXPECT_FLOAT_EQ(statistics_output->median, expected_output->median);
        EXPECT_NEAR(statistics_output->variance, expected_output->variance, FLOAT_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_variance, expected_output->first_der_variance, FIRST_DERIVATIVE_VARIANCE_ERROR_TOLERANCE);
    }
}

TEST(Statistics, PrecomputedCombinedStats) {
    /// Check parameters
    {
        assert(PRECOMPUTED_MEL_SPECTROGRAM_STATS_NUM_STATS == STATISTICS_ELEMENT_LENGTH);
        assert(PRECOMPUTED_MEL_SPECTROGRAM_N_MELS == PRECOMPUTED_MEL_SPECTROGRAM_STATS_N_MELS);
        assert(PRECOMPUTED_CEPSTRUM_COEFFICIENTS_NUM_STATS == STATISTICS_ELEMENT_LENGTH);
        assert(PRECOMPUTED_CEPSTRUM_COEFFICIENTS == PRECOMPUTED_CEPSTRUM_COEFFICIENTS_STATS);
        assert(PRECOMPUTED_COMBINED_STATS_NUM_STATS == STATISTICS_ELEMENT_LENGTH);
        assert(
            PRECOMPUTED_COMBINED_STATS_N_MELS == PRECOMPUTED_MEL_SPECTROGRAM_N_MELS &&
            PRECOMPUTED_COMBINED_STATS_N_MFCC == PRECOMPUTED_CEPSTRUM_COEFFICIENTS &&
            PRECOMPUTED_COMBINED_STATS_NUM_BINS == PRECOMPUTED_MEL_SPECTROGRAM_N_MELS + PRECOMPUTED_CEPSTRUM_COEFFICIENTS);
    }

    constexpr uint32_t output_buffer_length = PRECOMPUTED_COMBINED_STATS_NUM_BINS;
    struct statistics statistics_output_buffer[output_buffer_length];
    constexpr uint32_t MEL_SPECTROGRAM_STATS_START_ELEMENT = 0u;
    constexpr uint32_t CEPSTRUM_STATS_START_ELEMENT = PRECOMPUTED_COMBINED_STATS_N_MELS;

    /// Compute for mel spectrogram
    {
        constexpr uint32_t scratch_buffer_length = 2 * PRECOMPUTED_MEL_SPECTROGRAM_NUM_FRAMES * PRECOMPUTED_MEL_SPECTROGRAM_N_MELS;
        float scratch_array[scratch_buffer_length] = {};

        compute_statistics_across_frames(
            PRECOMPUTED_MEL_SPECTROGRAM_BUFFER,
            PRECOMPUTED_MEL_SPECTROGRAM_NUM_FRAMES,
            PRECOMPUTED_MEL_SPECTROGRAM_N_MELS,
            PRECOMPUTED_MEL_SPECTROGRAM_STATS_SAMPLE_RATE_RECIPROCAL,
            &statistics_output_buffer[MEL_SPECTROGRAM_STATS_START_ELEMENT],
            PRECOMPUTED_COMBINED_STATS_N_MELS,
            scratch_array,
            scratch_buffer_length
        );

        for (uint32_t output_iterator = 0; output_iterator < PRECOMPUTED_COMBINED_STATS_N_MELS; output_iterator++) {
            auto statistics_output =
                (const struct statistics*)&statistics_output_buffer[output_iterator + MEL_SPECTROGRAM_STATS_START_ELEMENT];
            auto expected_output =
                (const struct statistics*)&PRECOMPUTED_MEL_SPECTROGRAM_STATS_BUFFER[output_iterator * STATISTICS_ELEMENT_LENGTH];

            EXPECT_FLOAT_EQ(statistics_output->max, expected_output->max);
            EXPECT_FLOAT_EQ(statistics_output->min, expected_output->min);
            EXPECT_FLOAT_EQ(statistics_output->mean, expected_output->mean);
            EXPECT_FLOAT_EQ(statistics_output->median, expected_output->median);
            EXPECT_NEAR(statistics_output->variance, expected_output->variance, FLOAT_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_variance, expected_output->first_der_variance, FIRST_DERIVATIVE_VARIANCE_ERROR_TOLERANCE);
        }
    }

    /// Compute for cepstrum coefficients
    {
        constexpr uint32_t scratch_buffer_length = 2 * PRECOMPUTED_CEPSTRUM_NUM_FRAMES * PRECOMPUTED_CEPSTRUM_COEFFICIENTS;
        float scratch_array[scratch_buffer_length] = {};

        compute_statistics_across_frames(
        PRECOMPUTED_CEPSTRUM_BUFFER,
        PRECOMPUTED_CEPSTRUM_NUM_FRAMES,
        PRECOMPUTED_CEPSTRUM_COEFFICIENTS,
        PRECOMPUTED_CEPSTRUM_COEFFICIENTS_STATS_SAMPLE_RATE_RECIPROCAL,
        &statistics_output_buffer[CEPSTRUM_STATS_START_ELEMENT],
        PRECOMPUTED_CEPSTRUM_COEFFICIENTS,
        scratch_array,
        scratch_buffer_length);

        for (uint32_t output_iterator = 0; output_iterator < PRECOMPUTED_CEPSTRUM_COEFFICIENTS; output_iterator++) {
            auto statistics_output =
                (const struct statistics*)&statistics_output_buffer[output_iterator + CEPSTRUM_STATS_START_ELEMENT];
            auto expected_output =
                (const struct statistics*)&PRECOMPUTED_CEPSTRUM_STATS_BUFFER[output_iterator * STATISTICS_ELEMENT_LENGTH];

            EXPECT_FLOAT_EQ(statistics_output->max, expected_output->max);
            EXPECT_FLOAT_EQ(statistics_output->min, expected_output->min);
            EXPECT_FLOAT_EQ(statistics_output->mean, expected_output->mean);
            EXPECT_FLOAT_EQ(statistics_output->median, expected_output->median);
            EXPECT_NEAR(statistics_output->variance, expected_output->variance, FLOAT_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_variance, expected_output->first_der_variance, FIRST_DERIVATIVE_VARIANCE_ERROR_TOLERANCE);
        }
    }

    /// Compare output to combined stats buffer
    {
        for (uint32_t output_iterator = 0; output_iterator < output_buffer_length; output_iterator++) {
            auto statistics_output =
                (const struct statistics*)&statistics_output_buffer[output_iterator];
            auto expected_output =
                (const struct statistics*)&PRECOMPUTED_COMBINED_STATS_BUFFER[output_iterator * STATISTICS_ELEMENT_LENGTH];

            EXPECT_FLOAT_EQ(statistics_output->max, expected_output->max);
            EXPECT_FLOAT_EQ(statistics_output->min, expected_output->min);
            EXPECT_FLOAT_EQ(statistics_output->mean, expected_output->mean);
            EXPECT_FLOAT_EQ(statistics_output->median, expected_output->median);
            EXPECT_NEAR(statistics_output->variance, expected_output->variance, FLOAT_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_variance, expected_output->first_der_variance, FIRST_DERIVATIVE_VARIANCE_ERROR_TOLERANCE);
        }
    }
}
