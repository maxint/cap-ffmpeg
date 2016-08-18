@echo off
REM del /S /Q build-test
REM switch to current direcoty
pushd "%~dp0"
mkdir build-test
pushd build-test
cmake ..\test -G"NMake Makefiles"
nmake
copy /Y *.exe ..\install\bin\
popd
popd
pause
