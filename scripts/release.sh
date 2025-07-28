#!/bin/bash
set -e

BUILD_DIR="./bin"
FINAL_INSTALL_DIR="/usr/local/bin"
FINAL_DEPENDENCY_DIR="/usr/local/share/termify/bin"

echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

echo "Configuring CMake for Release..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      -S . -B "$BUILD_DIR"

echo "Linking compile_commands.json..."
ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json

echo "Building project..."
cmake --build "$BUILD_DIR" --config Release --target all

echo "Copying to final system locations (sudo may be required)..."
sudo install -Dm755 "$BUILD_DIR/termify" "$FINAL_INSTALL_DIR/termify"
sudo install -Dm755 "./ext/yt-dlp" "$FINAL_DEPENDENCY_DIR/yt-dlp"

echo "✅ Release installed to:"
echo "  - Binary: $FINAL_INSTALL_DIR/termify"
echo "  - yt-dlp: $FINAL_DEPENDENCY_DIR/yt-dlp"
