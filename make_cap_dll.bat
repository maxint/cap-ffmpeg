@echo off
set PREFIX=D:\mingw\msys\1.0\bin
set PATH=%PREFIX%;%PATH%
bash -c "cd src && make clean && make -j4 install && cd -"
mkdir build-test\Debug
mkdir build-test\Release
copy install\bin\ffmpeg_cap.dll build-test\Debug\
copy install\bin\ffmpeg_cap.dll build-test\Release\
pause
