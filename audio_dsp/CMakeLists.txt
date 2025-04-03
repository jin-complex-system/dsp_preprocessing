add_compile_definitions(TEST_ON_DESKTOP)

## Create shared library for desktop
add_library(
        audio_dsp_shared SHARED

        # Precomputed Constants
        include/hann_window_no_scale_1024.h
        include/hann_window_with_scale_1024.h

        include/audio_dsp_fft.h
        include/hann_window_compute.h
        include/power_spectrum.h

        src/audio_dsp_fft.c
        src/hann_window_compute.c
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
        unit_test/test_power_spectrum.cpp

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

