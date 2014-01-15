cmake_minimum_required(VERSION 2.6.3)

project(ffmpeg_cap_proj)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/_bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/_bin")
set(PREFIX "${CMAKE_SOURCE_DIR}/install")
if(DEFINED ENV{PREFIX})
  set(PREFIX "$ENV{PREFIX}")
endif()
set(CMAKE_MODULE_PATH "${PREFIX}/cmake")
set(CMAKE_INSTALL_PREFIX "${PREFIX}")

add_subdirectory(test)
