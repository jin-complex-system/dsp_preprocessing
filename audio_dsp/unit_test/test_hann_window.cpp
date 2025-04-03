#include <gtest/gtest.h>

#include <limits>
#include <cmath>
#include <power_spectrum.h>
#include <hann_window_compute.h>
#include <hann_window_no_scale_1024.h>
#include <hann_window_with_scale_1024.h>

// Empirically determined
constexpr double
FLOAT_ERROR_TOLERANCE = 5 * 1e-12;

constexpr auto
MAX_AUDIO_DATA_TYPE = std::numeric_limits<audio_data_type>::max();

TEST(HannWindow, ScalingAndNoScaling1024) {
    constexpr uint16_t N_FFT = 1024u;
    constexpr float SCALE_FACTOR = 1.0f / MAX_AUDIO_DATA_TYPE;
    constexpr double COS_VALUE = (2 * M_PI) / (N_FFT - 1);

    /// Check parameters
    {
        assert(HANN_WINDOW_SCALED_1024_LENGTH == N_FFT);
        assert(HANN_WIND0W_NO_SCALE_1024_LENGTH == N_FFT);
    }

    /// Call hann_window_compute()
    float hann_window_buffer_n_fft_scaled[N_FFT];
    hann_window_compute(
        hann_window_buffer_n_fft_scaled,
        N_FFT,
        SCALE_FACTOR
    );

    float hann_window_buffer_n_fft_no_scaled[N_FFT];
    hann_window_compute(
        hann_window_buffer_n_fft_no_scaled,
        N_FFT,
        1.0f
    );

    for (uint32_t bin_iterator = 0; bin_iterator < N_FFT; bin_iterator++) {
        EXPECT_NEAR(
            hann_window_buffer_n_fft_no_scaled[bin_iterator],
            HANN_WIND0W_NO_SCALE_1024[bin_iterator],
            FLOAT_ERROR_TOLERANCE);

        EXPECT_NEAR(
            hann_window_buffer_n_fft_scaled[bin_iterator],
            HANN_WINDOW_SCALED_1024[bin_iterator],
            FLOAT_ERROR_TOLERANCE);

        /// Compare Hann Window scale and no scale
        {
            const float hann_value_no_scale = (float)(0.5 - 0.5 * cos(COS_VALUE * (double)bin_iterator));
            const float hann_value_scaled = hann_value_no_scale * SCALE_FACTOR;

            EXPECT_NEAR(
                hann_value_scaled,
                HANN_WINDOW_SCALED_1024[bin_iterator],
                FLOAT_ERROR_TOLERANCE);
            EXPECT_NEAR(
                hann_value_no_scale,
                HANN_WIND0W_NO_SCALE_1024[bin_iterator],
                FLOAT_ERROR_TOLERANCE);
        }
    }
}
