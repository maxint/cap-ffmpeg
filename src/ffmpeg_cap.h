#ifndef __FFMPEG_H__
#define __FFMPEG_H__

#if defined(FFMPEG_EXPORTS)
#   if defined(_WINDOWS) || defined(__CYGWIN__)
#       define FFMPEG_DLL __declspec(dllexport)
#   elif defined(__GNUC__) && __GNUC__ >= 4
#       define FFMPEG_DLL __attribute__ ((visibility ("default")))
#   else
#       define FFMPEG_DLL
#   endif
#else
#   define FFMPEG_DLL
#endif

#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////
// Video Reader

enum {
    // read-write properties
    FFMPEG_PROP_FRAME_WIDTH=0,
    FFMPEG_PROP_FRAME_HEIGHT=1,
    FFMPEG_PROP_PIXEL_FORMAT=2,
    FFMPEG_PROP_POS_MSEC=3,
    FFMPEG_PROP_POS_FRAMES=4,
    FFMPEG_PROP_POS_AVI_RATIO=5,

    // read-only properties
    FFMPEG_PROP_ORIGINAL_FRAME_WIDTH=10,
    FFMPEG_PROP_ORIGINAL_FRAME_HEIGHT=11,
    FFMPEG_PROP_FOURCC=12,
    FFMPEG_PROP_FPS=13,
    FFMPEG_PROP_FRAME_COUNT=14,
    FFMPEG_PROP_BUFFER_SIZE=15,
};

typedef struct VideoCapture_FFMPEG VideoCapture_FFMPEG;

// @brief Create video reader.
FFMPEG_DLL VideoCapture_FFMPEG* ff_cap_create(const char* fname);

// @brief Release video reader.
FFMPEG_DLL void   ff_cap_release(VideoCapture_FFMPEG** cap);

// @brief Get property of video reader.
FFMPEG_DLL double ff_cap_get(VideoCapture_FFMPEG* cap, int propid);

// @brief Set property of video reader.
FFMPEG_DLL int    ff_cap_set(VideoCapture_FFMPEG* cap, int propid, double val);

// @brief Grab one frame from video stream.
FFMPEG_DLL int    ff_cap_grab(VideoCapture_FFMPEG* cap);

// @brief Retrieve and/or convert the grabbed video frame.
// @param data image planes.
// @param data width bytes of image planes.
FFMPEG_DLL int    ff_cap_retrieve(VideoCapture_FFMPEG* cap, const unsigned char* data[4], int step[4]);


//////////////////////////////////////////////////////////////////////////
// Video Writer

typedef struct VideoWriter_FFMPEG VideoWriter_FFMPEG;

// FFMPEG_FCC('Y800')
// http://www.fourcc.org/fourcc.php
#define FFMPEG_FCC(ch4) ((((unsigned int)(ch4) & 0xFF) << 24)    | \
                         (((unsigned int)(ch4) & 0xFF00) << 8)   | \
                         (((unsigned int)(ch4) & 0xFF0000) >> 8) | \
                         (((unsigned int)(ch4) & 0xFF000000) >> 24))

// @brief Create video writer.
// @param fourcc video encoder codec, created by FFMPEG_FCC(). Guess from file name if 0.
// @param src_pix_fmt input data pixel format, the same as data passed in ff_writer_write().
FFMPEG_DLL VideoWriter_FFMPEG* ff_writer_create(const char* fname, unsigned int fourcc, double fps, int width, int height, int src_pix_fmt);

// @brief Release video writer.
FFMPEG_DLL void ff_writer_release(VideoWriter_FFMPEG** writer);

// @brief Write one frame.
// @param data frame data
FFMPEG_DLL int  ff_writer_write(VideoWriter_FFMPEG* writer, const unsigned char* data);


//////////////////////////////////////////////////////////////////////////
// Utilities for pixel format

// @brief Return the size in bytes of the amount of data required to store an image with the given parameters.
// @see av_image_get_buffer_size(), avpicture_get_size()
FFMPEG_DLL int  ff_get_buffer_size(int pix_fmt, int width, int height);

// @brief Return the pixel format corresponding to name. Return -1 if no pixel format has been found.
// @param name "gray16", "nv21"
// @see av_get_pix_fmt()
FFMPEG_DLL int  ff_get_pix_fmt(const char *name);

// @brief Return the short name for a pixel format, NULL in case pix_fmt is unknown.
// @see av_get_pix_fmt_name()
FFMPEG_DLL const char* ff_get_pix_fmt_name(int pix_fmt);


//////////////////////////////////////////////////////////////////////////
// Convert and scale between pixel formats.

// swcale
enum {
    FF_SWS_FAST_BILINEAR=1,
    FF_SWS_BILINEAR     =2,
    FF_SWS_BICUBIC      =4,
    FF_SWS_X            =8,
    FF_SWS_POINT        =0x10,
    FF_SWS_AREA         =0x20,
    FF_SWS_BICUBLIN     =0x40,
    FF_SWS_GAUSS        =0x80,
    FF_SWS_SINC         =0x100,
    FF_SWS_LANCZOS      =0x200,
    FF_SWS_SPLINE       =0x400,
};
typedef struct SwsContext_FFMPEG SwsContext_FFMPEG;

/**
 * Check if context can be reused, otherwise reallocate a new one.
 *
 * If context is NULL, just calls sws_getContext() to get a new
 * context. Otherwise, checks if the parameters are the ones already
 * saved in context. If that is the case, returns the current
 * context. Otherwise, frees context and gets a new context with
 * the new parameters.
 *
 * Be warned that srcFilter and dstFilter are not checked, they
 * are assumed to remain the same.
 */
FFMPEG_DLL SwsContext_FFMPEG* ff_sws_getCachedContext(SwsContext_FFMPEG* ctx,
                                                      int srcW, int srcH, int srcFmt,
                                                      int dstW, int dstH, int dstFmt,
                                                      int flags);
/**
 * Free the swscaler context swsContext.
 * If swsContext is NULL, then does nothing.
 */
FFMPEG_DLL void ff_sws_freeContext(SwsContext_FFMPEG** ctx);

/**
 * Scale the image slice in srcSlice and put the resulting scaled
 * slice in the image in dst. A slice is a sequence of consecutive
 * rows in an image.
 *
 * Slices have to be provided in sequential order, either in
 * top-bottom or bottom-top order. If slices are provided in
 * non-sequential order the behavior of the function is undefined.
 *
 * @param c         the scaling context previously created with
 *                  sws_getContext()
 * @param srcSlice  the array containing the pointers to the planes of
 *                  the source slice
 * @param srcStride the array containing the strides for each plane of
 *                  the source image
 * @param dst       the array containing the pointers to the planes of
 *                  the destination image
 * @param dstStride the array containing the strides for each plane of
 *                  the destination image
 * @return          the height of the output slice
 */
FFMPEG_DLL int  ff_sws_scale(SwsContext_FFMPEG* ctx,
                             const unsigned char*const src[], const int srcStride[],
                             unsigned char* dst[], int dstStride[]);

#ifdef __cplusplus
}
#endif

#endif // __FFMPEG_H__
