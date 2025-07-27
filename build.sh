#!/bin/bash

BUILD_DIR="./bin"

if [[ -d "$BUILD_DIR" ]]; then
    rm -rf ./build
    mkdir build
    pushd build > /dev/null
    cmake .. 
    popd > /dev/null
fi

if [ -f compile_commands.json ]; then
    rm compile_commands.json
fi

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B "$BUILD_DIR"
ln -sf "./bin/compile_commands.json" compile_commands.json 
cmake --build "$BUILD_DIR"
