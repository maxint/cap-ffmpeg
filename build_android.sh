#!/bin/bash

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

#NDK=~/Desktop/android/android-ndk-r5b
#NDK=/home/NDK/android-ndk-r5b
NDK=/home/NDK/android-ndk-r8e
PLATFORM=$NDK/platforms/android-8/arch-arm
PREBUILT=$NDK/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86

function build_one
{
cd ffmpeg-2.1.1
./configure --target-os=linux \
    --prefix=$PREFIX \
    --enable-cross-compile \
    --extra-libs="-lgcc" \
    --arch=arm \
    --cc=$PREBUILT/bin/arm-linux-androideabi-gcc \
    --cross-prefix=$PREBUILT/bin/arm-linux-androideabi- \
    --nm=$PREBUILT/bin/arm-linux-androideabi-nm \
    --sysroot=$PLATFORM \
    --extra-cflags=" -O3 -fpic -DANDROID -DHAVE_SYS_UIO_H=1 -Dipv6mr_interface=ipv6mr_ifindex -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -finline-limit=300 $OPTIMIZE_CFLAGS " \
    --disable-shared \
    --enable-static \
    --extra-ldflags="-Wl,-rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib  -nostdlib -lc -lm -ldl -llog" \
    --disable-everything \
    --enable-demuxer=mov \
    --enable-demuxer=h264 \
    --disable-ffplay \
    --enable-protocol=file \
    --enable-avformat \
    --enable-avcodec \
    --enable-decoder=rawvideo \
    --enable-decoder=mjpeg \
    --enable-decoder=h263 \
    --enable-decoder=mpeg4 \
    --enable-decoder=h264 \
    --enable-parser=h264 \
    --disable-network \
    --enable-zlib \
    --disable-avfilter \
    --disable-avdevice \
    $ADDITIONAL_CONFIGURE_FLAG


make clean
make -j4 install

$PREBUILT/bin/arm-linux-androideabi-ar d libavcodec/libavcodec.a inverse.o

$PREBUILT/bin/arm-linux-androideabi-ld -rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib  -soname libffmpeg.so \
    -shared -nostdlib  -z,noexecstack -Bsymbolic --whole-archive --no-undefined -o $PREFIX/libffmpeg.so \
    libavcodec/libavcodec.a libavformat/libavformat.a libavutil/libavutil.a libswscale/libswscale.a \
    -lc -lm -lz -ldl -llog  --warn-once  --dynamic-linker=/system/bin/linker \
    $PREBUILT/lib/gcc/arm-linux-androideabi/4.4.3/libgcc.a

cd -
}

PREFIX=../android/$CPU 

#arm v6
#CPU=armv6
#OPTIMIZE_CFLAGS="-marm -march=$CPU"
#ADDITIONAL_CONFIGURE_FLAG=
#build_one

#arm v7vfpv3 (default)
#CPU=armv7-a
#OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfpv3-d16 -marm -march=$CPU "
#ADDITIONAL_CONFIGURE_FLAG=
#build_one

#arm v7vfp
#CPU=armv7-a
#OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "
#ADDITIONAL_CONFIGURE_FLAG=
#build_one

#arm v7n
CPU=armv7-a
OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=neon -marm -march=$CPU -mtune=cortex-a8"
ADDITIONAL_CONFIGURE_FLAG=--enable-neon
build_one

#arm v6+vfp
#CPU=armv6
#OPTIMIZE_CFLAGS="-DCMP_HAVE_VFP -mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU"
#ADDITIONAL_CONFIGURE_FLAG=
#build_one
