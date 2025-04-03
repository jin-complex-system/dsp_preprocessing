#include "audio_dsp_fft.h"
#include <assert.h>

void
perform_audio_dsp_rfft_direct(
    float* input_buffer,
    const uint16_t n_fft,
    float* output_buffer,
    const uint32_t output_buffer_length) {
    /// Check parameters
    assert(n_fft > 0 && n_fft % 2 == 0);
    assert(n_fft * 2 <= output_buffer_length);
    assert(input_buffer != NULL && output_buffer != NULL);

    audio_dsp_rfft_instance rfft_instance;
    initialise_audio_dsp_rfft(
        rfft_instance,
        n_fft);

    perform_audio_dsp_rfft(
        rfft_instance,
        input_buffer,
        n_fft,
        output_buffer,
        output_buffer_length
    );

    deinitialise_audio_dsp_rfft(rfft_instance);
}

void
initialise_audio_dsp_rfft(
    audio_dsp_rfft_instance *p_instance,
    const uint16_t n_fft) {
    /// Check parameters
    assert(n_fft > 0 && n_fft % 2 == 0);

#ifdef __ARM_ARCH
    arm_rfft_fast_init_f32(
        p_instance,
        n_fft);
#else
    // TODO: Implement kissFFT without the use of malloc
#endif //__ARM_ARCH

#ifdef TEST_ON_DESKTOP
    /// On desktop, we can use malloc
    *p_instance = kiss_fftr_alloc(
        n_fft,
        0,
        NULL,
        NULL);
#endif //TEST_ON_DESKTOP

    assert(p_instance != NULL);
}

void
perform_audio_dsp_rfft(
    audio_dsp_rfft_instance *p_instance,
    float* input_buffer,
    const uint32_t input_buffer_length,
    float* output_buffer,
    const uint32_t output_buffer_length) {
    /// Check parameters
    assert(p_instance != NULL);
    assert(input_buffer_length * 2 <= output_buffer_length);
    assert(input_buffer != NULL && output_buffer != NULL);

#ifdef __ARM_ARCH
    arm_rfft_fast_f32(
        p_instance,
        input_buffer,
        output_buffer,
        0);
#else

#endif //__ARM_ARCH

#ifdef TEST_ON_DESKTOP
    kiss_fftr(
        *p_instance,
        input_buffer,
        (kiss_fft_cpx*)output_buffer);
#endif // TEST_ON_DESKTOP
}



void
deinitialise_audio_dsp_rfft(
    audio_dsp_rfft_instance *p_instance) {
    if (p_instance != NULL) {
        // TODO: Implement

#ifdef __ARM_ARCH
#endif // __ARM_ARCH

#ifdef TEST_ON_DESKTOP
        kiss_fftr_free(*p_instance);
        p_instance = NULL;
#endif // TEST_ON_DESKTOP
    }

    // assert(p_instance == NULL);
}
