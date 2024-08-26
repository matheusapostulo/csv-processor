#!/bin/sh

# Checking whether GCC is installed or not
if ! command -v gcc &> /dev/null; then
    echo "Instaling gcc, musl-dev and binutils packages..."
    apk add build-base
fi

# Compiling the csv library
g++ -o libcsv.so -fpic -shared csv-processor/src/csv-processor.cpp