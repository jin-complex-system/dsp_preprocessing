## Library
add_library(
        utils SHARED

        include/convert_complex.h
        include/insertion_sort.h
        include/log_approximation.h
        include/square_root_approximation.h

        src/convert_complex.c
        src/insertion_sort.c
        src/log_approximation.c
        src/square_root_approximation.c
)

target_include_directories(
        utils PUBLIC

        include
)

## Unit Test
add_executable(
        UnitTestUtils

        unit_test/test_insertion_sort.cpp
        unit_test/test_log_approximation.cpp
        unit_test/test_logarithms_from_complex.cpp
        unit_test/test_magnitude_from_complex.cpp
        unit_test/test_square_root_approximation.cpp

        unit_test/constants/FFT_audio/rfft_output.h
        unit_test/constants/FFT_audio/rfft_log_magnitude.h
        unit_test/constants/FFT_audio/rfft_decibel_magnitude.h

        unit_test/constants/FFT_mels/rfft_output.h
        unit_test/constants/FFT_mels/rfft_log_magnitude.h
        unit_test/constants/FFT_mels/rfft_decibel_magnitude.h
)
target_include_directories(
        UnitTestUtils PUBLIC

        unit_test/constants
)
target_link_libraries(
        UnitTestUtils

        utils
        GTest::gtest_main
)

