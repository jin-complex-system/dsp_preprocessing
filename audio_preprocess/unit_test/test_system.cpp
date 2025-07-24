#include <gtest/gtest.h>

#include <audio_preprocess.h>

TEST(AudioPreprocess, Base) {
    constexpr struct audio_preprocess_parameters parameters = {
        .n_fft = 2048,
        .hop_length = 512,
        .n_mels = 64,
        .sample_rate = 44100u,
        .num_seconds = 7u,
        .max_frequency = 8000u,

        .num_cropped_frames = 0u,
        .left_padding = false,
    };

    // TODO: Figure out why program abruptly ends with the two comments commented in
    // Most likely linking problem

    // audio_preprocess_setup(&parameters);
    // audio_preprocess_deinit();
}
