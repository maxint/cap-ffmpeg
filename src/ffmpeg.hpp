#ifndef __FFMPEG_TK_HPP__
#define __FFMPEG_TK_HPP__

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}
#include "common/thread.hpp"
#include "common/utils.hpp"
#include "ffmpeg_cap.h"

struct VideoCapture_FFMPEG
{
    VideoCapture_FFMPEG()  { init(); }
    ~VideoCapture_FFMPEG() { destory(); }

    bool    open(const char* filename);
    void    close();

    bool    grabFrame();
    bool    retrieveFrame(uint8_t* data[4], int step[4]);

    double  getProperty(int);
    bool    setProperty(int, double);

    void    seek(int64_t frame_number);
    void    seek(double sec);

private:
    void    init();
    void    destory();
    bool set_target_picture(AVPixelFormat fmt, int width, int height);

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

    // target frame
    AVPixelFormat     dst_pix_fmt;
    int               dst_width, dst_height;
    SwsContext      * img_convert_ctx;
    AVFrame         * dst_frame;

    int64_t           frame_number;
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

    bool open(const char* filename, int fourcc, double fps, int width, int height, int pix_fmt);
    void close();
    bool writeFrame(const uint8_t* data);

private:
    void init();
    void destroy();

    AVOutputFormat  * out_fmt;
    AVFormatContext * format_ctx;
    AVFrame         * picture;
    uint8_t         * picbuf;
    AVStream        * video_st;

    AVPixelFormat     input_pix_fmt;
    int               frame_width, frame_height;
    AVFrame         * input_picture;

    bool              ok;
    SwsContext      * img_convert_ctx;
};

AVPixelFormat cvtfmt(int fmt);
int cvtfmt(AVPixelFormat fmt);

#endif /* end of include guard */
