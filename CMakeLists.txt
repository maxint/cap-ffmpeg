cmake_minimum_required(VERSION 2.6.3)

project(ffmpeg_cap)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/_bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/_bin")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install")

add_subdirectory(test)
