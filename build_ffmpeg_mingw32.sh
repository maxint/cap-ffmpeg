#!/bin/sh

TARGET_OS=mingw32

# Build ffmpeg static libraries

BUILD_DIR=build-$TARGET_OS
mkdir $BUILD_DIR -p
cd $BUILD_DIR

../ffmpeg-source/configure \
    --target-os=$TARGET_OS \
    --prefix="install" \
    --fatal-warnings \
    --enable-small \
    --disable-programs \
    --disable-doc \
    --disable-manpages \
    --disable-podpages \
    --disable-avdevice \
    --disable-avfilter \
    --disable-postproc \
    --disable-network \
    --disable-protocols \
    --enable-protocol=file \
    --enable-gpl \
    --enable-version3 \
    --enable-nonfree \
    --enable-avformat \
    --enable-avcodec \
    --enable-zlib \
    --enable-w32threads
    --enable-hwaccel=h263_vaapi \
    --enable-hwaccel=h263_vdpau \
    --enable-hwaccel=h264_dxva2 \
    --enable-hwaccel=h264_vaapi \
    --enable-hwaccel=h264_vda \
    --enable-hwaccel=h264_vdpau \
    --enable-hwaccel=mpeg4_vaapi \
    --enable-hwaccel=mpeg4_vdpau \
    --disable-encoders \
    --enable-encoder=libx264 \
    --enable-encoder=libxvid \
    --disable-muxers \
    --enable-muxer=mp4 \
    --enable-muxer=avi \

    #--disable-indevs \
    #--disable-outdevs \
    #--disable-filters \
    #--disable-bsfs \

make clean
make install

# echo "Striping static libraries..."
# cd install/lib 
# strip -g *.a

cd -
