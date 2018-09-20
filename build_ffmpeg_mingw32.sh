#!/bin/sh

TARGET_OS=mingw32

# Build x264 static libraries

BUILD_DIR=build-x264-$TARGET_OS
mkdir $BUILD_DIR -p
cd $BUILD_DIR

# ../x264-source/configure \
#     --prefix="install" \
#     --host=$TARGET_OS \
#     --enable-static \
#     --bit-depth=8 \
#     --disable-asm

# export PKG_CONFIG_PATH=$(readlink -f install/lib/pkgconfig)

# make install
cd -

# Build ffmpeg static libraries

BUILD_DIR=build-ffmpeg-$TARGET_OS
mkdir $BUILD_DIR -p
cd $BUILD_DIR

../ffmpeg-source/configure \
    --target-os=$TARGET_OS \
    --prefix="install" \
    --fatal-warnings \
    --extra-cflags="-Wno-format -Wno-unused-function -Wno-unused-const-variable -Wno-deprecated-declarations" \
    --disable-programs \
    --disable-doc \
    --disable-htmlpages \
    --disable-manpages \
    --disable-podpages \
    --disable-txtpages \
    --disable-avdevice \
    --disable-avfilter \
    --disable-postproc \
    --disable-network \
    --disable-encoders \
    --disable-muxers \
    --disable-protocols \
    --disable-indevs \
    --disable-outdevs \
    --disable-bsfs \
    --enable-gpl \
    --enable-version3 \
    --enable-nonfree \
    --enable-iconv \
    --enable-libwebp \
    --enable-protocol=file \
    --enable-avformat \
    --enable-avcodec \
    --enable-encoder=mjpeg \
    --enable-encoder=libopenh264 \
    --enable-encoder=libx264 \
    --enable-encoder=mpeg4 \
    --enable-muxer=avi \
    --enable-muxer=mp4 \
    --enable-muxer=rawvideo \
    --enable-muxer=mov \
    --enable-muxer=ogg \
    --enable-muxer=h264 \
    --disable-pthreads \

    # --enable-libx264 \
    # --enable-w32threads \
    # --enable-zlib \
    # --disable-indevs \
    # --disable-outdevs \
    # --disable-bsfs \

make install

# echo "Striping static libraries..."
# cd install/lib
# strip -g *.a

cd -
