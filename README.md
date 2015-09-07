INSTALL
=======

* Download the lastest [FFMPEG source](http://www.ffmpeg.org/download.html) to 'ffmpeg-source' directory
* Build ffmpeg and C wrapper (installed into 'install' directory)

- `make_ffmpeg.bat`: Build ffmpeg in MinGW.
- `make_cap_dll.bat`: Build `ffmpeg_cap` C wrapper in MinGW.
- `make_test_vs_project.bat`: Generate test Visual Studio project from
  `test/CMakeLists.txt`.
