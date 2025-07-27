cmake_minimum_required(VERSION 3.10)
project(termify)
set(CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)

# FFmpeg modules
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET
  libavformat libavcodec libavutil libswresample)

# ALSA
pkg_check_modules(ALSA REQUIRED alsa)

# Collect source files
file(GLOB_RECURSE SOURCES src/*.cpp)

# Define executable
add_executable(termify ${SOURCES})

# Include dirs
target_include_directories(termify PRIVATE
  ${FFMPEG_INCLUDE_DIRS}
  ${ALSA_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(termify
  PkgConfig::FFMPEG
  ${ALSA_LIBRARIES}
)
