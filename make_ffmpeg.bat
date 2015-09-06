@echo off
set MINGW_ROOT=D:\mingw
set PATH=%MINGW_ROOT%\bin;%MINGW_ROOT%\msys\1.0\bin;%MINGW_ROOT%\msys\1.0\opt\bin;%PATH%
%MINGW_ROOT%\msys\1.0\bin\bash.exe -c "bash build_mingw32.sh"
%MINGW_ROOT%\msys\1.0\bin\bash.exe -c "cd build-mingw32 && make clean && make -j4 install && cd install/lib strip -g *.a && cd - cd .."
pause
