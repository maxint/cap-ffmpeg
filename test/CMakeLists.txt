set(PROJNAME ffmpeg_cap_test)
add_executable(${PROJNAME}
  WIN32
  resource.h
  Resource.rc
  ffmpeg_cap_test.c)
 
find_package(ffmpeg_cap)
target_link_libraries(${PROJNAME} ffmpeg_cap)

install(TARGETS ${PROJNAME}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
