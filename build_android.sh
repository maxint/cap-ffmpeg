#!/bin/sh

######################################################
# Usage:
#   put this script in top of FFmpeg source tree
#   ./build_android
#
# It generates binary for following architectures:
#     ARMv6 
#     ARMv6+VFP 
#     ARMv7+VFPv3-d16 (Tegra2) 
#     ARMv7+Neon (Cortex-A8)
#
# Customizing:
# 1. Feel free to change ./configure parameters for more features
# 2. To adapt other ARM variants
#       set $CPU and $OPTIMIZE_CFLAGS 
#       call build_one
######################################################

TARGET_OS=android

BUILD_DIR=build-$TARGET_OS
mkdir $BUILD_DIR -p
cd $BUILD_DIR

#GCC_VER=4.7
#NDK=/home/NDK/android-ndk-r8e
#PREBUILT=$NDK/toolchains/arm-linux-androideabi-$GCC_VER/prebuilt/linux-x86

GCC_VER=4.9
NDK=/e/NDK/android-ndk-r10d
PREBUILT=$NDK/toolchains/arm-linux-androideabi-$GCC_VER/prebuilt/windows-x86_64

SYSROOT=$NDK/platforms/android-9/arch-arm
INSTALL_DIR=`pwd`/demo_fwk/jni/ffmpeg2
SRC_DIR=`pwd`/ffmpeg-2.1.1

CPU=armv7-a
PREFIX=install/$CPU 
OPTIMIZE_CFLAGS="-march=$CPU -mfloat-abi=softfp -mfpu=neon -mtune=cortex-a8"
ADDITIONAL_CONFIGURE_FLAG=--enable-neon

../ffmpeg-source/configure \
    --arch=arm \
    --target-os=linux \
    --enable-cross-compile \
    --cross-prefix=$PREBUILT/bin/arm-linux-androideabi- \
    --sysroot=$SYSROOT \
    --extra-cflags="$OPTIMIZE_CFLAGS" \
    --extra-libs="-lc -lm -ldl -llog -lgcc" \
    --prefix=$PREFIX \
    --fatal-warnings \
    --enable-gpl \
    --enable-version3 \
    --enable-nonfree \
    --disable-programs \
    --disable-doc \
    --disable-avdevice \
    --disable-avfilter \
    --disable-postproc \
    --disable-network \
    --disable-everything \
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
    $ADDITIONAL_CONFIGURE_FLAG


make clean
make -j4 install

$PREBUILT/bin/arm-linux-androideabi-ar d libavcodec/libavcodec.a inverse.o
$PREBUILT/bin/arm-linux-androideabi-ld --sysroot=$SYSROOT  -soname libffmpeg.so \
    -shared -z,noexecstack -Bsymbolic --whole-archive --no-undefined -o $PREFIX/libffmpeg.so \
    libavcodec/libavcodec.a libavformat/libavformat.a libavutil/libavutil.a libswscale/libswscale.a \
    -lc -lm -lz -ldl -llog $PREBUILT/lib/gcc/arm-linux-androideabi/$GCC_VER/libgcc.a

cd -
