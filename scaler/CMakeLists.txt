## Create static library
# Used for building upper-level libraries
add_library(
        scaler_static STATIC

        include/scaler_float.h

        src/scaler_float.c
)
target_include_directories(
        scaler_static PUBLIC

        include
)

## Create shared library
add_library(
        scaler_shared SHARED

        include/scaler_float.h

        src/scaler_float.c
)
target_include_directories(
        scaler_shared PUBLIC

        include
)

## Unit Test with GTest
add_executable(
        UnitTestScaler

        unit_test/test_scaler_float.cpp
)
target_include_directories(
        UnitTestScaler PUBLIC
)
target_link_libraries(
        UnitTestScaler

        scaler_shared
        GTest::gtest_main
)
add_test(
        NAME Scaler_CUnitTest
        COMMAND $<TARGET_FILE:UnitTestUtils>
)

# Python Test
add_test(
        NAME Scaler_PythonTest
        COMMAND ${Python_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/python_unit_test/test_scaler.py ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
