#!/bin/bash

# Build directory
BUILD_DIR=build

# If the build directory does not exist, create it
if [ ! -d "$BUILD_DIR" ]; then
  mkdir $BUILD_DIR
fi

# Compiling the shared object code
g++ -o $BUILD_DIR/libcsv-processor.so -fpic -shared src/csv-processor.cpp


# Finished message
echo "build_libcsv finished"
