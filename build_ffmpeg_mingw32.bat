@echo off
set MSYS_BIN_DIR=D:\mingw\msys\1.0\bin
set PATH=%MSYS_BIN_DIR%;%PATH%
bash build_mingw32.sh
pushd build-mingw32
REM make clean
REM make -j4 install
make install
echo "Striping static libraries..."
bash -c "cd install/lib && strip -g *.a"
popd
pause
