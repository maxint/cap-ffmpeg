#include "ffmpeg_cap.h"
#include "ffmpeg.hpp"

VideoCapture_FFMPEG* createCapture_FFMPEG(const char* fname)
{
    VideoCapture_FFMPEG *cap = new VideoCapture_FFMPEG();
    if (cap->open(fname))
        return cap;
    
    // clean up
    delete cap;
    return NULL;
}

void releaseCapture_FFMPEG(VideoCapture_FFMPEG** cap)
{
    if (cap && *cap) {
        delete *cap;
        *cap = NULL;
    }
}

int setCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid, double val)
{
    return cap->setProperty(propid, val);
}

double getCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid)
{
    return cap->getProperty(propid);
}

int grabFrame_FFMPEG(VideoCapture_FFMPEG* cap)
{
    return cap->grabFrame();
}

int retrieveFrame_FFMPEG(VideoCapture_FFMPEG* cap, unsigned char** data[4], int* step[4],
                         int* width, int* height)
{
    return cap->retrieveFrame(data, step, width, height);
}

//////////////////////////////////////////////////////////////////////////

VideoWriter_FFMPEG* createVideoWriter_FFMPEG(const char* fname, int fourcc, double fps,
                                             int width, int height, int isColor)
{
    VideoWriter_FFMPEG *writer = new VideoWriter_FFMPEG();
    if (writer->open(fname, fourcc, fps, width, height, isColor))
        return writer;
    delete writer;
    return NULL;
}

void releaseVideoWriter_FFMPEG(VideoWriter_FFMPEG** writer)
{
    if (writer && *writer) {
        delete *writer;
        *writer = NULL;
    }
}

int writeFrame_FFMPEG(VideoWriter_FFMPEG* writer, const unsigned char* data, int step,
                      int width, int height, int origin)
{
    return writer->writeFrame(data, step, width, height, origin);
}

