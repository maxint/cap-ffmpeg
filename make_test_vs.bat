mkdir build-test
pushd build-test
cmake ..\test
popd
echo "Copying ffmpeg_cap.dll to Debug and Release directories..."
pause
