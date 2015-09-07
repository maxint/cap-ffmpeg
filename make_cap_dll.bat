@echo off
set MSYS_BIN_DIR=D:\mingw\msys\1.0\bin
set PATH=%MSYS_BIN_DIR%;%PATH%
bash -c "cd src && make clean && make install && cd -"
mkdir build-test\Debug
mkdir build-test\Release
copy install\bin\ffmpeg_cap.dll build-test\Debug\
copy install\bin\ffmpeg_cap.dll build-test\Release\
pause
