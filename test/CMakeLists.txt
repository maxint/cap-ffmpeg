cmake_minimum_required(VERSION 2.8.12)

project(FFMPEG_CAP)

include_directories(../install/inc)

add_executable(player
  WIN32
  resource.h
  Resource.rc
  player.c)

add_executable(writer_test writer_test.cpp)
