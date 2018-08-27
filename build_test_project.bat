@echo off
REM del /S /Q build-test
mkdir build-test
pushd build-test
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
cmake ..\test -G"NMake Makefiles"
nmake
copy /Y *.exe ..\install\bin\
popd
pause
