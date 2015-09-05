cmake_minimum_required(VERSION 2.8.12)

project(ffmpeg_cap_test)

add_executable(${PROJECT_NAME}
  WIN32
  resource.h
  Resource.rc
  ffmpeg_cap_test.c)
target_include_directories(${PROJECT_NAME} PRIVATE ../install/inc)
