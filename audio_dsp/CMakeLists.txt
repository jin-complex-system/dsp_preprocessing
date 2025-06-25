add_compile_definitions(TEST_ON_DESKTOP)

## Create shared library for desktop
add_library(
        audio_dsp_shared SHARED

        # Precomputed Constants - Hann Window
        include/precomputed_window/hann_window/hann_window_no_scale_1024.h
        include/precomputed_window/hann_window/hann_window_scale_1024.h

        # Precomputed Constants - Mel float
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_32_fft_1024_sr_44100_fmax_8000.h

        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_float/mel_centre_frequencies_float_mel_64_fft_1024_sr_44100_fmax_8000.h

        # Precomputed Constants - Mel next bin
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_32_fft_1024_sr_44100_fmax_8000.h

        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_next_bin/mel_centre_frequencies_next_bin_mel_64_fft_1024_sr_44100_fmax_8000.h

        # Precomputed Constants - Mel prev bin
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_32_fft_1024_sr_44100_fmax_8000.h

        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_centre_frequencies_prev_bin/mel_centre_frequencies_prev_bin_mel_64_fft_1024_sr_44100_fmax_8000.h

        # Precomputed Constants - Mel weights
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_32_fft_1024_sr_44100_fmax_8000.h

        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_22048_fmax_0.h
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_22048_fmax_8000.h
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_44100_fmax_0.h
        include/precomputed_mel/mel_weights/mel_frequency_weights_mel_64_fft_1024_sr_44100_fmax_8000

        # Header files
        include/audio_dsp_fft.h
        include/hann_window_compute.h
        include/mel_spectrogram.h
        include/power_spectrum.h
        include/power_to_decibel.h

        # Source files
        src/audio_dsp_fft.c
        src/hann_window_compute.c
        src/mel_get_precomputed.c
        src/mel_spectrogram.c
        src/power_to_decibel.c
        src/mel_utils.c
        src/power_spectrum.c
)
target_link_libraries(
        audio_dsp_shared PUBLIC

        # Use static version
        statistics_static
        utils_static
        kissfft
)
target_include_directories(
        audio_dsp_shared PUBLIC

        include
)

## Unit Test
add_executable(
        UnitTestDSP

        unit_test/test_audio_dsp_fft.cpp
        unit_test/test_hann_window.cpp
        unit_test/test_mel_spectrogram.cpp
        unit_test/test_mel_precompute.cpp
        unit_test/test_power_spectrum.cpp
        unit_test/test_power_to_decibel.cpp

        unit_test/constants/power_spectrum_unit_test/power_spectrum_unit_test.h
        unit_test/constants/power_spectrum_unit_test/sinewave_input.h
        unit_test/constants/power_spectrum_unit_test/sinewave_rfft_decibel_magnitude.h

        unit_test/constants/test_audio/FFT_audio/rfft_input.h
        unit_test/constants/test_audio/FFT_audio/rfft_output.h
        unit_test/constants/test_audio/FFT_mels/rfft_input.h
        unit_test/constants/test_audio/FFT_mels/rfft_output.h
)
target_include_directories(
        UnitTestDSP PUBLIC

        unit_test/constants
)
target_link_libraries(
        UnitTestDSP PUBLIC

        audio_dsp_shared
        statistics_static
        utils_static
        GTest::gtest_main
)
add_test(
        NAME AudioDSP_UnitTest
        COMMAND $<TARGET_FILE:UnitTestDSP>
)

# Python Tests
add_test(
        NAME AudioDSP_FFT_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_audio_fft.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
        NAME AudioDSP_Hann_Window_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_hann_window.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
        NAME AudioDSP_PowerSpectrum_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_power_spectrum.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
        NAME AudioDSP_MelCompute_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_mel_compute.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
        NAME AudioDSP_MelSpectrogram_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_mel_spectrogram.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
        NAME AudioDSP_PowerToDecibel_PythonTestCase
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_power_to_decibel.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
