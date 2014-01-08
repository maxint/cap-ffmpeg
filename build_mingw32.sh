#!/bin/bash

mkdir mingw-build -p
cd mingw-build
../ffmpeg-2.1.1/configure \
    --target-os=mingw32 \
    --prefix="install"
    --extra-cflags="-O3 -fpic -fasm" \
    --extra-ldflags="" \
    --disable-shared \
    --disable-debug \
    --enable-static \
    #--disable-everything \
    #--enable-demuxer=mov \
    #--enable-demuxer=h264 \
    #--disable-ffplay \
    #--disable-ffprob \
    #--enable-protocol=file \
    #--enable-avformat \
    #--enable-avcodec \
    #--enable-decoder=rawvideo \
    #--enable-decoder=mjpeg \
    #--enable-decoder=h263 \
    #--enable-decoder=mpeg4 \
    #--enable-decoder=h264 \
    #--enable-parser=h264 \
    #--enable-network \
    #--enable-zlib \
    #--disable-avfilter \
    #--disable-avdevice \
    #--disable-filters \

make clean
make -j4 install

#$PREBUILT/bin/arm-linux-androideabi-ar d libavcodec/libavcodec.a inverse.o

#$PREBUILT/bin/arm-linux-androideabi-ld -rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib  -soname libffmpeg.so \
    #-shared -nostdlib  -z,noexecstack -Bsymbolic --whole-archive --no-undefined -o $PREFIX/libffmpeg.so \
    #libavcodec/libavcodec.a libavformat/libavformat.a libavutil/libavutil.a libswscale/libswscale.a \
    #-lc -lm -lz -ldl -llog  --warn-once  --dynamic-linker=/system/bin/linker \
    #$PREBUILT/lib/gcc/arm-linux-androideabi/4.4.3/libgcc.a

cd -
