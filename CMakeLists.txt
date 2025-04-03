cmake_minimum_required(VERSION 3.28)
project(dsp_preprocessing)

set(CMAKE_C_STANDARD 11)

# Python
find_package(Python COMPONENTS Interpreter REQUIRED)

## Unit Test with Googletest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(Pytest QUIET)
# TODO: Use Pytest-cmake

# Libraries
add_subdirectory(utils)
add_subdirectory(statistics)
add_subdirectory(audio_dsp)

# Add external libraries
add_subdirectory(external/kissfft)
