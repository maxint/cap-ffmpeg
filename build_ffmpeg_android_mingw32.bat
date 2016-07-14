@echo off
set MSYS_BIN_DIR=D:\mingw\msys\1.0\bin
set PATH=%MSYS_BIN_DIR%;%PATH%
bash build_android.sh
pushd build-android
REM make clean
REM make -j4 install
make install
popd
pause
