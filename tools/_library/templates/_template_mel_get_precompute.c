#include "mel_spectrogram.h"
#include <assert.h>
#include <stddef.h>

/// Generated library headers
/*[[[cog
import cog

# Global constants expected
assert  (len(data_structures) > 0)

# Include
for data_structure in data_structures:
    n_mel = data_structure.n_mel
    n_fft = data_structure.n_fft
    sample_rate = data_structure.sample_rate
    max_frequency = data_structure.max_frequency

    MEL_CENTRE_FLOAT_FILE = "mel_centre_frequencies_float_mel_{}_fft_{}_sr_{}_fmax_{}.h".format(
        n_mel, n_fft, sample_rate, max_frequency)
    MEL_CENTRE_NEXT_BIN_FILE = "mel_centre_frequencies_next_bin_mel_{}_fft_{}_sr_{}_fmax_{}.h".format(
        n_mel, n_fft, sample_rate, max_frequency)
    MEL_CENTRE_PREV_BIN_FILE = "mel_centre_frequencies_prev_bin_mel_{}_fft_{}_sr_{}_fmax_{}.h".format(
        n_mel, n_fft, sample_rate, max_frequency)
    MEL_WEIGHTS_FILE = "mel_frequency_weights_mel_{}_fft_{}_sr_{}_fmax_{}.h".format(
        n_mel, n_fft, sample_rate, max_frequency)

    cog.outl("#include <precomputed_mel/mel_centre_frequencies_float/{}>".format(MEL_CENTRE_FLOAT_FILE))
    cog.outl("#include <precomputed_mel/mel_centre_frequencies_next_bin/{}>".format(MEL_CENTRE_NEXT_BIN_FILE))
    cog.outl("#include <precomputed_mel/mel_centre_frequencies_prev_bin/{}>".format(MEL_CENTRE_PREV_BIN_FILE))
    cog.outl("#include <precomputed_mel/mel_weights/{}>".format(MEL_WEIGHTS_FILE))
    cog.outl(" ")

]]]*/
//[[[end]]]

/**
 * Assert that the precomputed buffer is as expected
 * @param precomputed_buffer
 * @param precomputed_buffer_length
 * @param expected_buffer_length
 */
static inline
void
assert_buffer(
    const void* precomputed_buffer,
    const uint32_t precomputed_buffer_length,
    const uint32_t expected_buffer_length)
{
#ifndef N_DEBUG
    assert(expected_buffer_length >= 1);
    assert(precomputed_buffer != NULL);
    assert(precomputed_buffer_length == expected_buffer_length);
#endif //N_DEBUG
}

/// Generated functions
/*[[[cog
for data_structure in data_structures:
    n_mel = data_structure.n_mel
    n_fft = data_structure.n_fft
    sample_rate = data_structure.sample_rate
    max_frequency = data_structure.max_frequency

    cog.outl("static inline")
    cog.outl("void")
    cog.outl("get_mel_precomputed_values_mel_{}_nfft_{}_sr_{}_fmax_{}(".format(
        n_mel, n_fft, sample_rate, max_frequency))
    cog.outl("    const uint16_t n_mel,")
    cog.outl("    const float** mel_centre_freq_float_buffer,")
    cog.outl("    const uint16_t** mel_centre_freq_next_bin_buffer,")
    cog.outl("    const uint16_t** mel_centre_freq_prev_bin_buffer,")
    cog.outl("    const float** mel_freq_weights_buffer) {")

    cog.outl("    /// Check parameters")
    cog.outl("    assert(*mel_centre_freq_float_buffer == NULL);")
    cog.outl("    assert(*mel_centre_freq_next_bin_buffer == NULL);")
    cog.outl("    assert(*mel_centre_freq_prev_bin_buffer == NULL);")
    cog.outl("    assert(*mel_freq_weights_buffer == NULL);")
    cog.outl("")

    mel_pre_str = "MEL_CENTRE_FREQUENCIES_FLOAT"
    mel_float_str = "{}_MEL_{}_FFT_{}_SR_{}_FMAX_{}_BUFFER".format(
        mel_pre_str, n_mel, n_fft, sample_rate, max_frequency)
    cog.outl("    assert_buffer(")
    cog.outl("        {},".format(mel_float_str))
    cog.outl("        {}_LENGTH,".format(mel_float_str))
    cog.outl("        n_mel + 1);")

    mel_pre_str = "MEL_CENTRE_FREQUENCIES_NEXT_BIN"
    mel_next_str = "{}_MEL_{}_FFT_{}_SR_{}_FMAX_{}_BUFFER".format(
        mel_pre_str, n_mel, n_fft, sample_rate, max_frequency)
    cog.outl("    assert_buffer(")
    cog.outl("        {},".format(mel_next_str))
    cog.outl("        {}_LENGTH,".format(mel_next_str))
    cog.outl("        n_mel - 1);")

    mel_pre_str = "MEL_CENTRE_FREQUENCIES_PREV_BIN"
    mel_prev_str = "{}_MEL_{}_FFT_{}_SR_{}_FMAX_{}_BUFFER".format(
        mel_pre_str, n_mel, n_fft, sample_rate, max_frequency)
    cog.outl("    assert_buffer(")
    cog.outl("        {},".format(mel_prev_str))
    cog.outl("        {}_LENGTH,".format(mel_prev_str))
    cog.outl("        n_mel - 1);")

    mel_pre_str = "MEL_FREQUENCY_WEIGHTS"
    mel_weights_str = "{}_MEL_{}_FFT_{}_SR_{}_FMAX_{}_BUFFER".format(
        mel_pre_str, n_mel, n_fft, sample_rate, max_frequency)
    cog.outl("    assert_buffer(")
    cog.outl("        {},".format(mel_weights_str))
    cog.outl("        {}_LENGTH,".format(mel_weights_str))
    cog.outl("        n_mel - 1);")

    # Actual assigning of weights
    cog.outl("    /// Assign values")
    cog.outl("    *mel_centre_freq_float_buffer = {};".format(mel_float_str))
    cog.outl("    *mel_centre_freq_next_bin_buffer = {};".format(mel_next_str))
    cog.outl("    *mel_centre_freq_prev_bin_buffer = {};".format(mel_prev_str))
    cog.outl("    *mel_freq_weights_buffer = {};".format(mel_weights_str))

    # End of function
    cog.outl("}")
    cog.outl("")

]]]*/
//[[[end]]]
const
bool
get_mel_spectrogram_precomputed_values(
    const uint16_t n_mel,
    const uint16_t n_fft,
    const uint16_t sample_rate,
    const uint16_t max_frequency,
    const float** mel_centre_freq_float_buffer,
    const uint16_t** mel_centre_freq_next_bin_buffer,
    const uint16_t** mel_centre_freq_prev_bin_buffer,
    const float** mel_freq_weights_buffer
) {
    /// Check parameters
    assert(n_mel >= 2 && n_mel % 2 == 0);
    assert(n_fft > 0 && n_fft % 2 == 0);
    assert(sample_rate > 0);
    assert(max_frequency <= sample_rate / 2 || max_frequency == 0u);
    assert(*mel_centre_freq_float_buffer == NULL);
    assert(*mel_centre_freq_next_bin_buffer == NULL);
    assert(*mel_centre_freq_prev_bin_buffer == NULL);
    assert(*mel_freq_weights_buffer == NULL);

    /// Locate precomputed values
    bool found_precomputed_value = false;

    /// Generated if conditionals
/*[[[cog

    for data_structure in data_structures:
        n_mel = data_structure.n_mel
        n_fft = data_structure.n_fft
        sample_rate = data_structure.sample_rate
        max_frequency = data_structure.max_frequency
        cog.outl("    if {}".format('('))
        cog.outl("        n_mel == {}u &&".format(n_mel))
        cog.outl("        n_fft == {}u &&".format(n_fft))
        cog.outl("        sample_rate == {}u && ".format(sample_rate, '{'))

        # Because max_frequency = 0 means sample_rate / 2, we need to add
        # an additional conditional statement
        if (max_frequency == 0):
            cog.outl("        (max_frequency == {}u || max_frequency == sample_rate / 2)) {}".format(max_frequency, '{'))
        else:
            cog.outl("        max_frequency == {}u) {}".format(max_frequency, '{'))
        cog.outl("")
        cog.outl("        assert(!found_precomputed_value);")
        cog.outl("")

        cog.outl("        get_mel_precomputed_values_mel_{}_nfft_{}_sr_{}_fmax_{}(".format(
            n_mel, n_fft, sample_rate, max_frequency))
        cog.outl("            n_mel,")
        cog.outl("            mel_centre_freq_float_buffer,")
        cog.outl("            mel_centre_freq_next_bin_buffer,")
        cog.outl("            mel_centre_freq_prev_bin_buffer,")
        cog.outl("            mel_freq_weights_buffer);")
        cog.outl("        found_precomputed_value = true;")
        cog.outl("    {}".format('}'))
        cog.outl("")

]]]*/
//[[[end]]]

#ifndef N_DEBUG
    if (found_precomputed_value)
    {
        assert(*mel_centre_freq_float_buffer != NULL);
        assert(*mel_centre_freq_next_bin_buffer != NULL);
        assert(*mel_centre_freq_prev_bin_buffer != NULL);
        assert(*mel_freq_weights_buffer != NULL);
    }
    else
    {
        assert(*mel_centre_freq_float_buffer == NULL);
        assert(*mel_centre_freq_next_bin_buffer == NULL);
        assert(*mel_centre_freq_prev_bin_buffer == NULL);
        assert(*mel_freq_weights_buffer == NULL);
    }

#endif // N_DEBUG

    return found_precomputed_value;
}
