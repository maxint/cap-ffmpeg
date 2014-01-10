include_directories(${CMAKE_SOURCE_DIR}/install/inc)
add_executable(ffmpeg_cap_test
  WIN32
  resource.h
  Resource.rc
  ffmpeg_cap_test.c)
target_link_libraries(ffmpeg_cap_test
  ${CMAKE_SOURCE_DIR}/install/lib/ffmpeg_cap.lib)

install(TARGETS ffmpeg_cap_test
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )