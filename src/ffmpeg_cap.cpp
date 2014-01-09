#include "ffmpeg_cap.h"
#include "ffmpeg.hpp"

VideoCapture_FFMPEG* cvCreateVideoCapture_FFMPEG(const char* fname)
{
    VideoCapture_FFMPEG *cap = new VideoCapture_FFMPEG();
    if (cap->open(fname))
        return cap;
    
    // clean up
    delete cap;
    return NULL;
}

void cvReleaseVideoCapture_FFMPEG(VideoCapture_FFMPEG** cap)
{
    if (cap && *cap) {
        delete *cap;
        *cap = NULL;
    }
}

int cvSetCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid, double val)
{
    return cap->setProperty(propid, val);
}

double cvGetCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid)
{
    return cap->getProperty(propid);
}

int cvGrabFrame_FFMPEG(VideoCapture_FFMPEG* cap)
{
    return cap->grabFrame();
}

int cvRetrieveFrame_FFMPEG(VideoCapture_FFMPEG* cap, unsigned char** data[4], int* step[4],
                           int* width, int* height)
{
    return cap->retrieveFrame(data, step, width, height);
}

//////////////////////////////////////////////////////////////////////////

VideoWriter_FFMPEG* cvCreateVideoWriter_FFMPEG(const char* fname, int fourcc, double fps,
                                               int width, int height, int isColor)
{
    VideoWriter_FFMPEG *writer = new VideoWriter_FFMPEG();
    if (writer->open(fname, fourcc, fps, width, height, isColor))
        return writer;
    delete writer;
    return NULL;
}

void cvReleaseVideoWriter_FFMPEG(VideoWriter_FFMPEG** writer)
{
    if (writer && *writer) {
        delete *writer;
        *writer = NULL;
    }
}

int cvWriteFrame_FFMPEG(VideoWriter_FFMPEG* writer, const unsigned char* data)
{
    return writer->writeFrame(data);
}

