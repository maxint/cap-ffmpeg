cmake_minimum_required(VERSION 2.6.3)

project(ffmpeg_cap_proj)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/_bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/_bin")
if(DEFINED ENV{PREFIX})
  set(CMAKE_MODULE_PATH "$ENV{PREFIX}/cmake")
  set(CMAKE_INSTALL_PREFIX "$ENV{PREFIX}")
endif()

add_subdirectory(test)
