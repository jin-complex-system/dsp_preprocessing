add_compile_definitions(TEST_ON_DESKTOP)

## Create shared library for desktop
add_library(
        audio_preprocess_shared SHARED

        include/audio_preprocess.h
        src/audio_preprocess.c
)
target_link_libraries(
        audio_preprocess_shared PUBLIC

        # Use static version
        audio_dsp_static
        statistics_static
        utils_static
        kissfft
)
target_include_directories(
        audio_preprocess_shared PUBLIC

        include
)

## Unit Test with GTest
add_executable(
        UnitTestAudioPreprocess

        unit_test/test_system.cpp
        unit_test/test_example_human_voice.cpp

        unit_test/constants/audio/human_voice.h
)
target_include_directories(
        UnitTestAudioPreprocess PUBLIC

        unit_test/constants/
)
target_link_libraries(
        UnitTestAudioPreprocess

        # Use static version
        audio_preprocess_shared
        audio_dsp_static
        statistics_static
        utils_static
        scaler_static
        GTest::gtest_main
)
add_test(
        NAME AudioPreprocess_CUnitTest
        COMMAND $<TARGET_FILE:UnitTestAudioPreprocess>
)

# Python Test
add_test(
        NAME AudioPreprocess_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_system.py ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

