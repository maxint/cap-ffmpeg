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
    --disable-pthreads \
    --disable-encoders \
    --disable-muxers \
    --enable-gpl \
    --enable-version3 \
    --enable-nonfree \
    --enable-protocol=file \
    --enable-avformat \
    --enable-avcodec \
    --enable-parser=h264 \
    --enable-parser=mjpeg \
    --enable-parser=mpeg4video \
    --enable-demuxer=rawvideo \
    --enable-demuxer=h264 \
    --enable-demuxer=mjpeg \
    --enable-demuxer=mpegvideo \
    --enable-demuxer=mov \
    --enable-decoder=rawvideo \
    --enable-decoder=h264 \
    --enable-decoder=mjpeg \
    --enable-decoder=mpeg4 \
    --enable-encoder=rawvideo \
    --enable-encoder=mjpeg \
    --enable-encoder=h264_amf \
    --enable-encoder=h264_nvenc \
    --enable-encoder=h264_omx \
    --enable-encoder=h264_qsv \
    --enable-encoder=h264_v4l2m2m \
    --enable-encoder=h264_vaapi \
    --enable-encoder=h264_videotoolbox \
    --enable-encoder=libopenh264 \
    --enable-encoder=libx264 \
    --enable-encoder=nvenc_h264 \
    --enable-encoder=mpeg4 \
    --enable-muxer=rawvideo \
    --enable-muxer=mp4 \
    --enable-muxer=mov \
    --enable-muxer=ogg \
    --enable-muxer=h264 \
    --enable-hwaccel=h264_d3d11va \
    --enable-hwaccel=h264_d3d11va2 \
    --enable-hwaccel=h264_dxva2 \
    --enable-hwaccel=h264_nvdec \
    --enable-hwaccel=h264_vaapi \
    --enable-hwaccel=h264_vdpau \
    --enable-hwaccel=h264_videotoolbox \
    --enable-hwaccel=mpeg4_nvdec \
    --enable-hwaccel=mpeg4_vaapi \
    --enable-hwaccel=mpeg4_vdpau \
    --enable-hwaccel=mpeg4_videotoolbox \

    # --enable-w32threads
    # --enable-zlib \
    # --disable-indevs \
    # --disable-outdevs \
    # --disable-bsfs \

make clean
make install

# echo "Striping static libraries..."
# cd install/lib
# strip -g *.a

cd -
