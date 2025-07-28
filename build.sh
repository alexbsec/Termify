#!/bin/bash

BUILD_DIR="./bin"

if [[ -d "$BUILD_DIR" ]]; then
    rm -rf ./bin
    mkdir bin
    pushd bin > /dev/null
    cmake .. 
    popd > /dev/null
fi

if [ -f compile_commands.json ]; then
    rm compile_commands.json
fi

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B "$BUILD_DIR"
ln -sf "./bin/compile_commands.json" compile_commands.json 
mkdir -p ./bin/.local/bin
cp ./ext/yt-dlp ./bin/.local/bin/yt-dlp
cmake --build "$BUILD_DIR"
