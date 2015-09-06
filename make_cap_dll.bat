@echo off
set MINGW_ROOT=D:\mingw
set PATH=%MINGW_ROOT%\bin;%MINGW_ROOT%\msys\1.0\bin;%MINGW_ROOT%\msys\1.0\opt\bin;%PATH%
%MINGW_ROOT%\msys\1.0\bin\bash.exe -c "cd src && make clean && make -j4 install"
mkdir build-test\Debug
mkdir build-test\Release
copy install\bin\ffmpeg_cap.dll build-test\Debug\
copy install\bin\ffmpeg_cap.dll build-test\Release\
pause
