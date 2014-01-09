#!/bin/bash

TARGET_OS=mingw32

BUILD_DIR=$TARGET_OS-build
rm $BUILD_DIR -rf
mkdir $BUILD_DIR -p
cd $BUILD_DIR
../ffmpeg-2.1.1/configure \
    --target-os=$TARGET_OS \
    --prefix="install" \
    --fatal-warnings \
    --extra-cflags="-O3 -fasm" \
    --enable-pic \
    --extra-ldflags="" \
    --enable-gpl \
    --enable-version3 \
    --enable-nonfree \
    --disable-shared \
    --disable-debug \
    --enable-static \
    --disable-programs \
    --disable-doc \
    --disable-avdevice \
    --disable-avfilter \
    --disable-postproc \
    --disable-avfilter \
    --disable-network \
    --disable-everything \
    --enable-demuxer=mov \
    --enable-demuxer=h264 \
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
    --enable-muxer=mp4 \
    --enable-muxer=mov \
    --enable-muxer=ogg \
    --enable-muxer=h264 \
    --enable-muxer=rawvideo \
    --enable-zlib \

make clean
make -j4 install

#$PREBUILT/bin/arm-linux-androideabi-ar d libavcodec/libavcodec.a inverse.o

#$PREBUILT/bin/arm-linux-androideabi-ld -rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib  -soname libffmpeg.so \
    #-shared -nostdlib  -z,noexecstack -Bsymbolic --whole-archive --no-undefined -o $PREFIX/libffmpeg.so \
    #libavcodec/libavcodec.a libavformat/libavformat.a libavutil/libavutil.a libswscale/libswscale.a \
    #-lc -lm -lz -ldl -llog  --warn-once  --dynamic-linker=/system/bin/linker \
    #$PREBUILT/lib/gcc/arm-linux-androideabi/4.4.3/libgcc.a

cd -
