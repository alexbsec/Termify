#!/bin/bash

BUILD_DIR="./bin/Debug"

if [[ -d "$BUILD_DIR" ]]; then
    rm -rf "$BUILD_DIR"
    mkdir "$BUILD_DIR"
    pushd "$BUILD_DIR" > /dev/null
    cmake ..
    popd > /dev/null
    popd > /dev/null
fi

if [ -f compile_commands.json ]; then
    rm compile_commands.json
fi

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B "$BUILD_DIR"
ln -sf "./bin/Debug/compile_commands.json" compile_commands.json 
mkdir -p ./bin/Debug/.local/bin
cp ./ext/yt-dlp ./bin/Debug/.local/bin/yt-dlp
cmake --build "$BUILD_DIR"
