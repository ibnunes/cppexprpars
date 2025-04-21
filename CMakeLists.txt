cmake_minimum_required(VERSION 3.10)

# Project name
project(cppexprpars VERSION 1.0 LANGUAGES CXX)

# Enable C++14 (or higher) for compatibility
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create the main library
add_library(cppexprpars STATIC
    include/cppexprpars.hpp
    src/expr_node.cpp
    src/function.cpp
    src/cppexprpars.cpp
)

# Include headers for the library
target_include_directories(cppexprpars PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Optionally, add tests
enable_testing()

# Only include test files if the build type is Debug or when explicitly requested
if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_executable(test_cppexprpars
        tests/test.cpp
    )
    add_executable(test_quick_example
        tests/quick_example.cpp
    )

    # Link the library to the test executable
    target_link_libraries(test_cppexprpars PRIVATE cppexprpars)
    target_link_libraries(test_quick_example PRIVATE cppexprpars)

    # Add a basic test
    add_test(NAME test_cppexprpars COMMAND test_cppexprpars)
endif()
