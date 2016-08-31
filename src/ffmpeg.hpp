//////////////////////////////////////////////////////////////////////////
// Simple C Wrapper of FFMPEG (Version >= 2.1.1)
// Author: maxint <maxint@foxmail.com>
//////////////////////////////////////////////////////////////////////////

#ifndef __FFMPEG_TK_HPP__
#define __FFMPEG_TK_HPP__

// Current used ffmpeg version: 2.8
extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
} // extern "C"

#include <stdint.h>
#include "ffmpeg_cap.h"

struct VideoCapture_FFMPEG
{
    VideoCapture_FFMPEG()  { init(); }
    ~VideoCapture_FFMPEG() { destory(); }

    bool    open(const char* filename);
    void    close();

    bool    grabFrame();
    bool    retrieveFrame(const uint8_t* data[4], int step[4]);
    const uint8_t* retrieveFrame();

    double  getProperty(int);
    bool    setProperty(int, double);

    void    seek(int64_t frame_number);
    void    seek(double sec);

private:
    void    init();
    void    destory();
    bool    set_target_picture(AVPixelFormat fmt, int width, int height);

    int64_t get_total_frames();
    double  get_duration_sec();
    double  get_fps();
    int     get_bitrate();

    int64_t dts_to_frame_number(int64_t dts);
    double  dts_to_sec(int64_t dts);

    AVFormatContext * format_ctx;
    int               video_stream; // video stream id
    AVStream        * video_st;
    AVCodecContext  * vcodec_ctx;
    AVFrame         * src_frame;
    int64_t           picture_pts;
    AVPacket          packet;

    // target frame
    AVPixelFormat     dst_pix_fmt;
    int               dst_width, dst_height;
    SwsContext      * img_convert_ctx;
    AVFrame         * dst_frame;
    int               buf_size;

    int64_t           frame_number;
    int64_t           frame_count;
    int64_t           first_frame_number;

/*
   'filename' contains the filename of the video source,
   'filename==NULL' indicates that ffmpeg's seek support works for the particular file.
   'filename!=NULL' indicates that the slow fallback function is used for seeking,
    and so the filename is needed to reopen the file on backward seeking.
*/
    char              * filename;
};

struct VideoWriter_FFMPEG
{
    VideoWriter_FFMPEG()  { init(); }
    ~VideoWriter_FFMPEG() { close(); }

    bool open(const char* filename, unsigned fourcc, double fps, int width, int height, AVPixelFormat src_pix_fmt);
    void close();
    bool writeFrame(const uint8_t* data);

private:
    void init();
    void destroy();

    AVOutputFormat  * oformat;
    AVFormatContext * format_ctx;
    AVFrame         * dst_picture;
    uint8_t         * picbuf;
    AVStream        * video_st;

    AVPixelFormat     input_pix_fmt;
    int               frame_width, frame_height;
    AVFrame         * input_picture;

    bool              ok;
    SwsContext      * img_convert_ctx;
};

#endif /* end of include guard */
