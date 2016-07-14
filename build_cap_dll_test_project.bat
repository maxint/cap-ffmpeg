@echo off
REM del /S /Q build-test
mkdir build-test
pushd build-test
cmake ..\test -G"NMake Makefiles"
nmake
copy /Y *.exe ..\install\bin\
popd
pause
