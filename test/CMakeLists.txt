cmake_minimum_required(VERSION 2.8.12)

project(FFMPEG_CAP)

# save libs and executables in the same place
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}" CACHE PATH "Output directory for applications")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "")   # static
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} CACHE INTERNAL "") # shared
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH} CACHE INTERNAL "") # exe

include_directories(../install/inc)

add_executable(player
  WIN32
  resource.h
  resource.rc
  player.c)

add_executable(writer_test writer_test.cpp)
