@echo off
set PREFIX=D:\mingw\msys\1.0\bin
set PATH=%PREFIX%;%PATH%
REM bash build_mingw32.sh
pushd build-mingw32
REM make clean
REM make -j4 install
make install
echo "Striping static libraries..."
bash -c "cd install/lib && strip -g *.a"
popd
pause
