## Create static library
# Used for building upper-level libraries
add_library(
        statistics_static SHARED

        include/statistics.h

        src/statistics.c
)
target_link_libraries(
        statistics_static PUBLIC

        utils_static # Use static version
)
target_include_directories(
        statistics_static PUBLIC

        include
)

## Create shared library
add_library(
        statistics_shared SHARED

        include/statistics.h

        src/statistics.c
)
target_link_libraries(
        statistics_shared PUBLIC

        utils_static # Use static version
)
target_include_directories(
        statistics_shared PUBLIC

        include
)

## Unit Test
add_executable(
        UnitTestStatistics

        unit_test/test_statistics.cpp

        unit_test/constants/cepstrum.h
        unit_test/constants/cepstrum_stats.h
        unit_test/constants/mel_spectrogram.h
        unit_test/constants/mel_spectrogram_stats.h
)
target_include_directories(
        UnitTestStatistics PUBLIC

        unit_test/constants
)
target_link_libraries(
        UnitTestStatistics PUBLIC

        statistics_shared
        utils_static
        GTest::gtest_main
)
add_test(
        NAME Statistics_UnitTest
        COMMAND $<TARGET_FILE:UnitTestStatistics>
)

# Python Test
add_test(
        NAME Statistics_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_statistics.py ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
