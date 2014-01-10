#include "ffmpeg_cap.h"
#include "ffmpeg.hpp"

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

int ff_cap_retrieve(VideoCapture_FFMPEG* cap, unsigned char* data[4], int step[4])
{
    return cap->retrieveFrame(data, step);
}

//////////////////////////////////////////////////////////////////////////

VideoWriter_FFMPEG* ff_writer_create(const char* fname, int fourcc, double fps,
                                     int width, int height, int pix_fmt)
{
    VideoWriter_FFMPEG *writer = new VideoWriter_FFMPEG();
    if (writer->open(fname, fourcc, fps, width, height, pix_fmt))
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

struct SwsContext_FFMPEG
{
    SwsContext_FFMPEG() {
        img_convert_ctx = NULL;
        height = 0;
    }
    ~SwsContext_FFMPEG() {
        if (img_convert_ctx) sws_freeContext(img_convert_ctx);
    }

    bool getCached(
        int srcW, int srcH, int srcFmt,
        int dstW, int dstH, int dstFmt,
        int flags)
    {
        img_convert_ctx = sws_getCachedContext(
            img_convert_ctx,
            srcW, srcH, cvtfmt(srcFmt),
            dstW, dstH, cvtfmt(dstFmt),
            flags,
            NULL, NULL, NULL
            );
        height = srcH;
        return img_convert_ctx != NULL;
    }

    int scale(
        const uint8_t *const src[], const int srcStride[], 
        uint8_t* dst[], int dstStride[])
    {
        return sws_scale(
            img_convert_ctx,
            src, srcStride,
            0,
            height,
            dst, dstStride);
    }
private:
    int height;
    SwsContext* img_convert_ctx;
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
