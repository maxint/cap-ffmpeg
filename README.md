Introduction
=======

Simple FFMPEG C wrapper for reading and writing of video files.
The FFMPEG source code and this C wrapper is built by MinGW.
The main code is from [OpenCV](http://www.opencv.org) and [ffmpeg tutorial by Stephen Dranger](http://dranger.com/ffmpeg/).


Install
=======

* Download the latest [x264 source](ftp://ftp.videolan.org/pub/videolan/x264/snapshots/) to `x264-source` directory.
* Download the latest [FFMPEG source](http://www.ffmpeg.org/download.html) to `ffmpeg-source` directory
* Build ffmpeg and C wrapper (installed into 'install' directory) in [MinGW](http://www.mingw.org/)

    - `build_ffmpeg_mingw32.sh`: Build libx264 and ffmpeg in MinGW
    - `build_cap_ffmpeg_mingw32.sh`: Build `ffmpeg_cap` C wrapper in MinGW
    - `build_test_project.bat`: Build CMake test project by VS2017


Directories
===========

- [`src`](src): C wrapper
- [`test/player.c`](test/player.c): simple player of C wrapper in Windows.
- [`test/writer_test.cpp`](test/writer_test.cpp): Video writer test in Windows.

