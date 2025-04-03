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
FIRST_DERIVATIVE_STD_DEV_ERROR_TOLERANCE = 5.0;

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
        EXPECT_FLOAT_EQ(statistics_output.std_dev, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_mean, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_std_dev, 0.0f);
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
        EXPECT_FLOAT_EQ(statistics_output.std_dev, 0.4f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_mean, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output.first_der_std_dev, 0.70710677f);
    }

    /// Another typical example
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr auto INPUT_LENGTH = 3u;
        constexpr float input_array[INPUT_LENGTH] = {
            1, 4, 0,
        };
        constexpr uint32_t SCRATCH_LENGTH = 2 * INPUT_LENGTH;
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

        EXPECT_FLOAT_EQ(statistics_output.max, 4.0f);
        EXPECT_FLOAT_EQ(statistics_output.min, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output.mean, (1.0 + 4.0 + 0.0)/3.0);
        EXPECT_FLOAT_EQ(statistics_output.median, 1.0f);
        EXPECT_FLOAT_EQ(statistics_output.std_dev, 1.6996732f);
    }
}

TEST(Statistics, ComputeStatsAcrossFrame) {
    /// Basic example 1
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr auto FRAME_LENGTH = 5u;
        constexpr auto BIN_LENGTH = 1u;
        constexpr float input_array[FRAME_LENGTH] = {
            0.0f,   // Frame 1
            0.0f,   // Frame 2
            1.0f,   // Frame 3
            0.0f,   // Frame 4
            0.0f,   // Frame 5
        };
        constexpr uint32_t SCRATCH_LENGTH = 2 * FRAME_LENGTH;
        constexpr uint32_t OUTPUT_LENGTH = BIN_LENGTH;
        float scratch_array[SCRATCH_LENGTH] = {};
        struct statistics statistics_output[OUTPUT_LENGTH] = {};

        compute_statistics_across_frames(
            input_array,
            FRAME_LENGTH,
            BIN_LENGTH,
            FRAME_DIFFERENCE_RECIPROCAL,
            statistics_output,
            OUTPUT_LENGTH,
            scratch_array,
            SCRATCH_LENGTH
        );
        EXPECT_FLOAT_EQ(statistics_output->max, 1.0f);
        EXPECT_FLOAT_EQ(statistics_output->min, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output->mean, 1.0f / FRAME_LENGTH);
        EXPECT_FLOAT_EQ(statistics_output->median, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output->std_dev, 0.4f);
        // EXPECT_FLOAT_EQ(statistics_output->first_der_mean, 0.0f);
        // EXPECT_FLOAT_EQ(statistics_output->first_der_std_dev, 0.70710677f);
    }

    /// Basic example 2
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr auto NUM_FRAMES = 10u;
        constexpr auto BIN_LENGTH = 1u;
        constexpr float input_array[NUM_FRAMES] = {
            0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        };
        constexpr uint32_t SCRATCH_LENGTH = 2 * NUM_FRAMES;
        constexpr uint32_t OUTPUT_LENGTH = BIN_LENGTH;
        float scratch_array[SCRATCH_LENGTH] = {};
        struct statistics statistics_output[OUTPUT_LENGTH] = {};

        compute_statistics_across_frames(
            input_array,
            NUM_FRAMES,
            BIN_LENGTH,
            FRAME_DIFFERENCE_RECIPROCAL,
            statistics_output,
            OUTPUT_LENGTH,
            scratch_array,
            SCRATCH_LENGTH
        );
        EXPECT_FLOAT_EQ(statistics_output->max, 2.0f);
        EXPECT_FLOAT_EQ(statistics_output->min, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output->mean, (1.0f + 2.0f) / NUM_FRAMES);
        EXPECT_FLOAT_EQ(statistics_output->median, 0.0f);
        EXPECT_FLOAT_EQ(statistics_output->std_dev, 0.64031243f);
    }

    /// Typical example
    {
        constexpr float FRAME_DIFFERENCE_RECIPROCAL = 1.0f;
        constexpr uint32_t NUM_FRAMES = 4u;
        constexpr uint32_t NUM_BINS = 3u;
        constexpr uint32_t INPUT_LENGTH = NUM_FRAMES * NUM_BINS;
        constexpr float input_array[INPUT_LENGTH] = {
            1, 4, 0,            // Frame 1
            2, 5, 9,            // Frame 2
            3, -2, 0,           // Frame 3
            8, 0, 0,            // Frame 4
        };
        assert(sizeof(input_array) == INPUT_LENGTH * sizeof(float));
        constexpr uint32_t SCRATCH_LENGTH = 2 * INPUT_LENGTH;
        constexpr uint32_t OUTPUT_LENGTH = NUM_BINS;

        constexpr struct statistics EXPECTED_RESULT[OUTPUT_LENGTH] = {
            {8.0, 1.0, (1.0 + 2.0 + 3.0 + 8.0)/NUM_FRAMES, 2.5, 2.69258237f},     // Bin 1
            {5.0, -2.0, (4.0 + 5.0 + -2.0 + 0.0)/NUM_FRAMES, 2.0, 2.8613808f},           // Bin 2
            {9.0, 0.0, (0.0 + 9.0 + 0.0 + 0.0)/NUM_FRAMES, 0.0, 3.8971143f},          // Bin 3
        };

        float scratch_array[SCRATCH_LENGTH] = {};
        struct statistics statistics_output[OUTPUT_LENGTH] = {};

        compute_statistics_across_frames(
            input_array,
            NUM_FRAMES,
            NUM_BINS,
            FRAME_DIFFERENCE_RECIPROCAL,
            statistics_output,
            OUTPUT_LENGTH,
            scratch_array,
            SCRATCH_LENGTH
        );

        for (uint32_t bin_iterator = 0; bin_iterator < NUM_BINS; bin_iterator++) {
            const struct statistics &computed_stats = statistics_output[bin_iterator];
            const struct statistics &expected_stats = EXPECTED_RESULT[bin_iterator];

            EXPECT_FLOAT_EQ(computed_stats.max, expected_stats.max);
            EXPECT_FLOAT_EQ(computed_stats.min, expected_stats.min);
            EXPECT_FLOAT_EQ(computed_stats.mean, expected_stats.mean);
            EXPECT_FLOAT_EQ(computed_stats.median, expected_stats.median);
            EXPECT_FLOAT_EQ(computed_stats.std_dev, expected_stats.std_dev);
        }
    }
}

TEST(Statistics, CompareStatsMethods) {
        /// Use compute_statistics_within_frame() to compare results
    {
        constexpr uint32_t BIN_LENGTH = 5u;
        constexpr uint32_t NUM_FRAMES = 3u;

        constexpr uint32_t
        INPUT_LENGTH = BIN_LENGTH * NUM_FRAMES;

        constexpr uint32_t
        SCRATCH_LENGTH = 2 * NUM_FRAMES;

        constexpr uint32_t
        OUTPUT_LENGTH = BIN_LENGTH;

        assert(INPUT_LENGTH > 0);

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
            EXPECT_FLOAT_EQ(across_frames_statistics_output->std_dev, within_frames_statistics_output.std_dev);
            // EXPECT_FLOAT_EQ(across_frames_statistics_output->first_der_mean, within_frames_statistics_output.first_der_mean);
            // EXPECT_FLOAT_EQ(across_frames_statistics_output->first_der_std_dev, within_frames_statistics_output.first_der_std_dev);
        }
    }
}

// TODO: Implement hard-coded examples
/*
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
        EXPECT_NEAR(statistics_output->std_dev, expected_output->std_dev, FLOAT_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_std_dev, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_std_dev, expected_output->first_der_std_dev, FIRST_DERIVATIVE_STD_DEV_ERROR_TOLERANCE);
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
        EXPECT_NEAR(statistics_output->std_dev, expected_output->std_dev, FLOAT_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
        // EXPECT_NEAR(statistics_output->first_der_std_dev, expected_output->first_der_std_dev, FIRST_DERIVATIVE_STD_DEV_ERROR_TOLERANCE);
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
            EXPECT_NEAR(statistics_output->std_dev, expected_output->std_dev, FLOAT_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_std_dev, expected_output->first_der_std_dev, FIRST_DERIVATIVE_STD_DEV_ERROR_TOLERANCE);
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
            EXPECT_NEAR(statistics_output->std_dev, expected_output->std_dev, FLOAT_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_std_dev, expected_output->first_der_std_dev, FIRST_DERIVATIVE_STD_DEV_ERROR_TOLERANCE);
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
            EXPECT_NEAR(statistics_output->std_dev, expected_output->std_dev, FLOAT_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_mean, expected_output->first_der_mean, FIRST_DERIVATIVE_MEAN_ERROR_TOLERANCE);
            // EXPECT_NEAR(statistics_output->first_der_std_dev, expected_output->first_der_std_dev, FIRST_DERIVATIVE_STD_DEV_ERROR_TOLERANCE);
        }
    }
}
*/
