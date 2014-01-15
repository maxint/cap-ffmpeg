add_library(ffmpeg_cap SHARED IMPORTED)
set_target_properties(ffmpeg_cap PROPERTIES
  IMPORTED_LOCATION $ENV{PREFIX}/bin/ffmpeg_cap.dll
  IMPORTED_IMPLIB $ENV{PREFIX}/lib/ffmpeg_cap.lib
  INTERFACE_INCLUDE_DIRECTORIES $ENV{PREFIX}/include
  )
