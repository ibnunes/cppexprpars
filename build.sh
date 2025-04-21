#!/bin/bash

# Set the root directory of the project (where CMakeLists.txt is located)
ROOT_DIR=$(pwd)

# Create and navigate to the build directory
BUILD_DIR="${ROOT_DIR}/build"
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Default build type is Release if not provided
BUILD_TYPE="Release"

# Check if the user has provided a build type argument (Debug or Release)
if [ "$1" == "Debug" ] || [ "$1" == "Release" ] || [ "$1" == "RelWithDebInfo" ]; then
    BUILD_TYPE="$1"
else
    echo "Invalid build type specified. Using default: Release."
fi

# Run CMake to generate the build files
echo "Generating build files with CMake (Build Type: $BUILD_TYPE)..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE "$ROOT_DIR" || { echo "CMake configuration failed!"; exit 1; }

# Build the project
echo "Building the project (Build Type: $BUILD_TYPE)..."
cmake --build . || { echo "Build failed!"; exit 1; }

# Run the tests (if any are defined and it's a Debug/RelWithDebInfo build)
if [ "$BUILD_TYPE" == "Debug" ] || [ "$BUILD_TYPE" == "RelWithDebInfo" ]; then
    echo "Running tests..."
    ctest --output-on-failure || { echo "Tests failed!"; exit 1; }
else
    echo "Skipping tests (Build Type: $BUILD_TYPE)."
fi

echo "Build and tests completed successfully!"
