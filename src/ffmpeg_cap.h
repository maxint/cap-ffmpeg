#ifndef __FFMPEG_CAP_H__
#define __FFMPEG_CAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CV_FFMPEG_EXPORTS
#   define CV_FFMPEG_DLL __declspec(dllexport)
#else
#   define CV_FFMPEG_DLL __declspec(dllimport)
#endif

struct VideoCapture_FFMPEG;
struct VideoWriter_FFMPEG;

inline int FFMPEG_FOURCC(char c1, char c2, char c3, char c4) {
    return (c1 & 255) + ((c2 & 255) << 8) + ((c3 & 255) << 16) + ((c4 & 255) << 24);
}

enum {
    FFMPEG_CAP_PROP_POS_MSEC=0,
    FFMPEG_CAP_PROP_POS_FRAMES=1,
    FFMPEG_CAP_PROP_POS_AVI_RATIO=2,
    FFMPEG_CAP_PROP_FPS=3,
    FFMPEG_CAP_PROP_FRAME_WIDTH=4,
    FFMPEG_CAP_PROP_FRAME_HEIGHT=5,
    FFMPEG_CAP_PROP_PIXEL_FORMAT=6,

    // read-only
    FFMPEG_CAP_PROP_FOURCC=7,
    FFMPEG_CAP_PROP_FRAME_COUNT=8,
    FFMPEG_CAP_PROP_ORIGINAL_FRAME_WIDTH=9,
    FFMPEG_CAP_PROP_ORIGINAL_FRAME_HEIGHT=10,
};

// Video Capture
CV_FFMPEG_DLL VideoCapture_FFMPEG* cvCreateVideoCapture_FFMPEG(const char* fname);
CV_FFMPEG_DLL void cvReleaseVideoCapture_FFMPEG(VideoCapture_FFMPEG** cap);
CV_FFMPEG_DLL int cvSetCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid, double val);
CV_FFMPEG_DLL double cvGetCaptureProperty_FFMPEG(VideoCapture_FFMPEG* cap, int propid);
CV_FFMPEG_DLL int cvGrabFrame_FFMPEG(VideoCapture_FFMPEG* cap);
CV_FFMPEG_DLL int cvRetrieveFrame_FFMPEG(VideoCapture_FFMPEG* cap,
                                         unsigned char** data[4], int* step[4],
                                         int* width, int* height);

// Video Writer
CV_FFMPEG_DLL VideoWriter_FFMPEG* cvCreateVideoWriter_FFMPEG(
    const char* fname, int fourcc, double fps, int width, int height, int isColor);
CV_FFMPEG_DLL void cvReleaseVideoWriter_FFMPEG(VideoWriter_FFMPEG** writer);
CV_FFMPEG_DLL int cvWriteFrame_FFMPEG(VideoWriter_FFMPEG* writer, const unsigned char* data);

#ifdef __cplusplus
}
#endif

#endif // __FFMPEG_CAP_H__
