## Create static library
# Used for building upper-level libraries
add_library(
        utils_static STATIC

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
        utils_static PUBLIC

        include
)

## Create shared library
add_library(
        utils_shared SHARED

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
        utils_shared PUBLIC

        include
)

## Unit Test with GTest
add_executable(
        UnitTestUtils

        unit_test/test_insertion_sort.cpp
        unit_test/test_log_approximation.cpp
        unit_test/test_logarithms_from_complex.cpp
        unit_test/test_magnitude_and_power_from_complex.cpp
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

        utils_shared
        GTest::gtest_main
)
add_test(
        NAME Utils_CUnitTest
        COMMAND $<TARGET_FILE:UnitTestUtils>
)

# Python Test
add_test(
        NAME Utils_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_utils.py ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
add_test(
        NAME Utils_PlotErrors
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/plot_errors.py ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
