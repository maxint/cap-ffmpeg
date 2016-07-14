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
    --disable-programs \
    --disable-doc \
    --disable-avdevice \
    --disable-avfilter \
    --disable-postproc \
    --disable-network \
    --disable-everything \
    --enable-gpl \
    --enable-version3 \
    --enable-nonfree \
    --enable-protocol=file \
    --enable-avformat \
    --enable-avcodec \
    --enable-decoder=rawvideo \
    --enable-decoder=mjpeg \
    --enable-decoder=h263 \
    --enable-decoder=mpeg4 \
    --enable-decoder=h264 \
    --enable-encoder=rawvideo \
    --enable-encoder=mjpeg \
    --enable-encoder=h263 \
    --enable-encoder=libx264 \
    --enable-encoder=mpeg4 \
    --enable-hwaccel=h263_vaapi \
    --enable-hwaccel=h263_vdpau \
    --enable-hwaccel=h264_dxva2 \
    --enable-hwaccel=h264_vaapi \
    --enable-hwaccel=h264_vda \
    --enable-hwaccel=h264_vdpau \
    --enable-hwaccel=mpeg4_vaapi \
    --enable-hwaccel=mpeg4_vdpau \
    --enable-parser=mjpeg \
    --enable-parser=h263 \
    --enable-parser=h264 \
    --enable-parser=mpeg4video \
    --enable-demuxer=mov \
    --enable-demuxer=h264 \
    --enable-muxer=mp4 \
    --enable-muxer=mov \
    --enable-muxer=ogg \
    --enable-muxer=h264 \
    --enable-muxer=rawvideo \
    --enable-zlib \
    --enable-w32threads

#make clean
#make -j4 install

cd -
