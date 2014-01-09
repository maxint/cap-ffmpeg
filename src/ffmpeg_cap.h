#ifndef __FFMPEG_CAP_H__
#define __FFMPEG_CAP_H__

#ifdef FFMPEG_CAP_EXPORTS
#   define FFMPEG_CAP_DLL __declspec(dllexport)
#else
#   define FFMPEG_CAP_DLL __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
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
FFMPEG_CAP_DLL VideoCapture_FFMPEG* ff_cap_create(const char* fname);
FFMPEG_CAP_DLL void   ff_cap_release(VideoCapture_FFMPEG** cap);
FFMPEG_CAP_DLL double ff_cap_get(VideoCapture_FFMPEG* cap, int propid);
FFMPEG_CAP_DLL int    ff_cap_set(VideoCapture_FFMPEG* cap, int propid, double val);
FFMPEG_CAP_DLL int    ff_cap_grab(VideoCapture_FFMPEG* cap);
FFMPEG_CAP_DLL int    ff_cap_retrieve(VideoCapture_FFMPEG* cap,
                                      unsigned char** data[4], int* step[4]);

// Video Writer
FFMPEG_CAP_DLL VideoWriter_FFMPEG* ff_writer_create(const char* fname,
                                                    int fourcc, double fps,
                                                    int width, int height, int pix_fmt);
FFMPEG_CAP_DLL void ff_writer_release(VideoWriter_FFMPEG** writer);
FFMPEG_CAP_DLL int  ff_writer_write(VideoWriter_FFMPEG* writer, const unsigned char* data);

#ifdef __cplusplus
}
#endif

#endif // __FFMPEG_CAP_H__
