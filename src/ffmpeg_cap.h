#ifndef __FFMPEG_CAP_H__
#define __FFMPEG_CAP_H__

struct VideoCapture_FFMPEG;
struct VideoWriter_FFMPEG;

inline int FFMPEG_FOURCC(char c1, char c2, char c3, char c4) {
    return (c1 & 255) + ((c2 & 255) << 8) + ((c3 & 255) << 16) + ((c4 & 255) << 24);
}

enum {
    FFMPEG_CAP_PROP_POS_MSEC=0,
    FFMPEG_CAP_PROP_POS_FRAMES=1,
    FFMPEG_CAP_PROP_POS_AVI_RATIO=2,
    FFMPEG_CAP_PROP_FRAME_WIDTH=3,
    FFMPEG_CAP_PROP_FRAME_HEIGHT=4,
    FFMPEG_CAP_PROP_FPS=5,
    FFMPEG_CAP_PROP_PIXEL_FORMAT=10,

    // read-only
    FFMPEG_CAP_PROP_FOURCC=6,
    FFMPEG_CAP_PROP_FRAME_COUNT=7,
    FFMPEG_CAP_PROP_ORIGINAL_FRAME_WIDTH=8,
    FFMPEG_CAP_PROP_ORIGINAL_FRAME_HEIGHT=9,
};

// Video Capture
VideoCapture_FFMPEG* createCapture_FFMPEG(const char* fname);
void releaseCapture_FFMPEG(VideoCapture_FFMPEG** cap);
int setCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid, double val);
double getCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid);
int grabFrame_FFMPEG(VideoCapture_FFMPEG* cap);
int retrieveFrame_FFMPEG(VideoCapture_FFMPEG* cap,
                         unsigned char** data[4], int* step[4],
                         int* width, int* height);


// Video Writer
VideoWriter_FFMPEG* createVideoWriter_FFMPEG(const char* fname, int fourcc, double fps,
                                             int width, int height, int isColor);
void releaseVideoWriter_FFMPEG(VideoWriter_FFMPEG** writer);
int writeFrame_FFMPEG(VideoWriter_FFMPEG* writer,
                      const unsigned char* data, int step,
                      int width, int height, int origin);


#endif // __FFMPEG_CAP_H__
