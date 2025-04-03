add_compile_definitions(TEST_ON_DESKTOP)

## Create shared library for desktop
add_library(
        audio_dsp_shared SHARED

        include/audio_dsp_fft.h

        src/audio_dsp_fft.c
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

        unit_test/constants/FFT_audio/rfft_input.h
        unit_test/constants/FFT_audio/rfft_output.h
        unit_test/constants/FFT_mels/rfft_input.h
        unit_test/constants/FFT_mels/rfft_output.h
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

# Python Test
add_test(
        NAME SAudioDSP_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_audio_fft.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
