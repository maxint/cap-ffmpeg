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

// FFMPEG_FCC('Y800')
#define FFMPEG_FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
                         (((DWORD)(ch4) & 0xFF00) << 8) |    \
                         (((DWORD)(ch4) & 0xFF0000) >> 8) |  \
                         (((DWORD)(ch4) & 0xFF000000) >> 24))

enum {
    // read-write properties
    FFMPEG_CAP_PROP_FRAME_WIDTH=0,
    FFMPEG_CAP_PROP_FRAME_HEIGHT=1,
    FFMPEG_CAP_PROP_PIXEL_FORMAT=2,
    FFMPEG_CAP_PROP_POS_MSEC=3,
    FFMPEG_CAP_PROP_POS_FRAMES=4,
    FFMPEG_CAP_PROP_POS_AVI_RATIO=5,

    // read-only properties
    FFMPEG_CAP_PROP_ORIGINAL_FRAME_WIDTH=10,
    FFMPEG_CAP_PROP_ORIGINAL_FRAME_HEIGHT=11,
    FFMPEG_CAP_PROP_FOURCC=12,
    FFMPEG_CAP_PROP_FPS=13,
    FFMPEG_CAP_PROP_FRAME_COUNT=14,
};

// Video Capture
FFMPEG_CAP_DLL VideoCapture_FFMPEG* ff_cap_create(const char* fname);
FFMPEG_CAP_DLL void   ff_cap_release(VideoCapture_FFMPEG** cap);
FFMPEG_CAP_DLL double ff_cap_get(VideoCapture_FFMPEG* cap, int propid);
FFMPEG_CAP_DLL int    ff_cap_set(VideoCapture_FFMPEG* cap, int propid, double val);
FFMPEG_CAP_DLL int    ff_cap_grab(VideoCapture_FFMPEG* cap);
FFMPEG_CAP_DLL int    ff_cap_retrieve(VideoCapture_FFMPEG* cap,
                                      unsigned char* data[4], int step[4]);

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
