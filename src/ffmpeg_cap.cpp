#include "ffmpeg_cap.h"
#include "ffmpeg.hpp"

#define LOG_TAG "ffmpeg"
#include "common/log.hpp"

extern "C" {
	#include <libavutil/imgutils.h>
} // extern "C"

VideoCapture_FFMPEG* ff_cap_create(const char* fname)
{
    VideoCapture_FFMPEG *cap = new VideoCapture_FFMPEG();
    if (cap->open(fname))
        return cap;
    
    // clean up
    delete cap;
    return NULL;
}

void ff_cap_release(VideoCapture_FFMPEG** cap)
{
    if (cap && *cap) {
        delete *cap;
        *cap = NULL;
    }
}

int ff_cap_set(VideoCapture_FFMPEG* cap, int propid, double val)
{
    return cap->setProperty(propid, val);
}

double ff_cap_get(VideoCapture_FFMPEG* cap, int propid)
{
    return cap->getProperty(propid);
}

int ff_cap_grab(VideoCapture_FFMPEG* cap)
{
    return cap->grabFrame();
}

int ff_cap_retrieve(VideoCapture_FFMPEG* cap, const unsigned char* data[4], int step[4])
{
    return cap->retrieveFrame(data, step);
}

//////////////////////////////////////////////////////////////////////////

VideoWriter_FFMPEG* ff_writer_create(const char* fname, unsigned int fourcc, double fps, int width, int height, int src_pix_fmt)
{
    VideoWriter_FFMPEG *writer = new VideoWriter_FFMPEG();
    if (writer->open(fname, fourcc, fps, width, height, (AVPixelFormat) src_pix_fmt))
        return writer;
    delete writer;
    return NULL;
}

void ff_writer_release(VideoWriter_FFMPEG** writer)
{
    if (writer && *writer) {
        delete *writer;
        *writer = NULL;
    }
}

int ff_writer_write(VideoWriter_FFMPEG* writer, const unsigned char* data)
{
    return writer->writeFrame(data);
}

//////////////////////////////////////////////////////////////////////////

int ff_get_buffer_size(int pix_fmt, int width, int height)
{
	return avpicture_get_size((AVPixelFormat) pix_fmt, width, height);
}

int ff_get_pix_fmt(const char *name)
{
	return (int) av_get_pix_fmt(name);
}

const char* ff_get_pix_fmt_name(int pix_fmt)
{
	return av_get_pix_fmt_name((AVPixelFormat) pix_fmt);
}

//////////////////////////////////////////////////////////////////////////

/*
struct SwsContext_FFMPEG
{
    SwsContext_FFMPEG() {
        img_convert_ctx = NULL;
        height = 0;
        dst_width = dst_height = 0;
        dst_fmt = 0;
        memset(&dst_picture, 0, sizeof(dst_picture));
        need_cvt = false;
    }
    ~SwsContext_FFMPEG() {
        if (img_convert_ctx) sws_freeContext(img_convert_ctx);
        if (dst_picture.data[0]) avpicture_free(&dst_picture);
    }

    bool getCached(
        int srcW, int srcH, int srcFmt,
        int dstW, int dstH, int dstFmt,
        int flags)
    {
        need_cvt = srcW != dstW || srcH != dstH || srcFmt != dstFmt;
        if (need_cvt) {
            img_convert_ctx = sws_getCachedContext(
                img_convert_ctx,
                srcW, srcH, srcFmt,
                dstW, dstH, dstFmt,
                flags,
                NULL, NULL, NULL
                );
            if (!img_convert_ctx)
                return false;
            if (dst_picture.data[0]) avpicture_free(&dst_picture);
            if (avpicture_alloc(&dst_picture, dstFmt, dstW, dstH) != 0)
                return false;
        }
        dst_width = dstW;
        dst_height = dstH;
        dst_fmt = dstFmt;
        return true;
    }

    int scale(
        const uint8_t *const src[4], const int srcStride[4], 
        uint8_t* dst[4], int dstStride[4]=NULL)
    {
        if (!dst && !dstStride)
            return -1;

        if (!img_convert_ctx)
            return -1;

        if (need_cvt) {
            int goth = sws_scale(
                img_convert_ctx,
                src, srcStride,
                0,
                height,
                dst_picture.data,
                dst_picture.linesize);
            if (goth != dst_height)
                return false;

            memcpy(dst, dst_picture.data, sizeof(uint8_t*) * 4);
            if (dstStride)
                memcpy(dstStride, dst_picture.linesize, sizeof(int) * 4);
            return goth;
        } else {
            memcpy(dst, src, sizeof(uint8_t*) * 4);
            if (dstStride)
                memcpy(dstStride, srcStride, sizeof(int) * 4);
            return dst_height;
        }
    }
private:
    int height;
    int dst_width;
    int dst_height;
    int dst_fmt;
    bool need_cvt;
    SwsContext* img_convert_ctx;
    AVPicture dst_picture;
};

SwsContext_FFMPEG* ff_sws_getCachedContext(SwsContext_FFMPEG* ctx,
                                           int srcW, int srcH, int srcFmt,
                                           int dstW, int dstH, int dstFmt,
                                           int flags)
{
    if (!ctx)
        ctx = new SwsContext_FFMPEG();
    if (ctx->getCached(srcW, srcH, srcFmt, dstW, dstH, dstFmt, flags))
        return ctx;
    delete ctx;
    return NULL;
}

void ff_sws_freeContext(SwsContext_FFMPEG** ctx)
{
    if (ctx && *ctx) {
        delete *ctx;
        *ctx = NULL;
    }
}

int ff_sws_scale(SwsContext_FFMPEG* ctx,
                 const uint8_t *const src[], const int srcStride[],
                 uint8_t* dst[], int dstStride[])
{
    return ctx->scale(src, srcStride, dst, dstStride);
}
*/
