## Library
add_library(
        statistics SHARED

        include/statistics.h

        src/statistics.c
)
target_link_libraries(
        statistics

        utils
)

target_include_directories(
        utils PUBLIC

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
        UnitTestStatistics

        statistics
        utils
        GTest::gtest_main
)

