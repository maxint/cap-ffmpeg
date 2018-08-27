//////////////////////////////////////////////////////////////////////////
// Simple C Wrapper of FFMPEG (Version >= 2.1.1)
// Author: maxint <maxint@foxmail.com>
// License: GNU General Public License (https://www.gnu.org/copyleft/gpl.html)
//////////////////////////////////////////////////////////////////////////

#include "ffmpeg.hpp"
#include <math.h>       /* floor */

extern "C" {
#   include <libavutil/imgutils.h>
} // extern "C"

#define DUMP_DEBUG_INFO 0
#define NO_FUNC_LOG

// #pragma GCC diagnostic push
// #pragma GCC diagnostic pop
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

//////////////////////////////////////////////////////////////////////////
// Include these macros here for independent code.
#define LOG_TAG "ffmpeg"
#if defined(__ANDROID__) || defined(ANDROID)
#  	include <android/log.h>
#  	define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG, ##__VA_ARGS__)
#  	define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,  LOG_TAG, ##__VA_ARGS__)
#  	define LOGI(...) __android_log_print(ANDROID_LOG_INFO,   LOG_TAG, ##__VA_ARGS__)
#  	define LOGW(...) __android_log_print(ANDROID_LOG_WARN,   LOG_TAG, ##__VA_ARGS__)
#  	define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,  LOG_TAG, ##__VA_ARGS__)
#else
#  	include <stdio.h>
#  	define _LOG_(L, fmt, ...) fprintf(stderr, "%s/%s: " fmt "\n", LOG_TAG, L, ##__VA_ARGS__)
#  	define LOGV(fmt, ...) _LOG_("V", fmt, ##__VA_ARGS__)
#  	define LOGD(fmt, ...) _LOG_("D", fmt, ##__VA_ARGS__)
#  	define LOGI(fmt, ...) _LOG_("I", fmt, ##__VA_ARGS__)
#  	define LOGW(fmt, ...) _LOG_("W", fmt, ##__VA_ARGS__)
#  	define LOGE(fmt, ...) _LOG_("E", fmt, ##__VA_ARGS__)
#endif
#ifndef NO_FUNC_LOG
#   define ENTER_FUNCTION	LOGV("[in ] %s:%s", __FILE__, __FUNCTION__)
#   define EXIT_FUNCTION	LOGV("[out] %s:%s", __FILE__, __FUNCTION__)
#	define QLOG				LOGD("%s(%d):%s", __FILE__, __LINE__, __FUNCTION__)
#else
#   define ENTER_FUNCTION
#   define EXIT_FUNCTION
#	define QLOG
#endif	//end of NO_JNI_LOG
#undef max
#undef min
template<typename T> T max(T a, T b) { return a > b ? a : b; }
template<typename T> T min(T a, T b) { return a < b ? a : b; }
//////////////////////////////////////////////////////////////////////////

static inline void dump_all_iformat()
{
    LOGD("All input formats:");
    AVInputFormat *fmt = NULL;
    LOGD("- Name | Long Name | Extensions");
    while ((fmt = av_iformat_next(fmt))) {
        LOGD("- %s | %s | %s", fmt->name, fmt->long_name, fmt->extensions);
    }
}

static inline void dump_all_oformat()
{
    LOGD("All output formats:");
    AVOutputFormat *fmt = NULL;
    LOGD("- Name | Long Name | Mime-Type | Extensions");
    while ((fmt = av_oformat_next(fmt))) {
        LOGD("- %s | %s | %s | %s", fmt->name, fmt->long_name, fmt->mime_type, fmt->extensions);
    }
}

//////////////////////////////////////////////////////////////////////////
// NOTE: for bug in ffmpeg-2.1.1:
// auto detect the output format from file name and fourcc code.
// e.g.
//      oformat = av_guess_format(NULL, filename, NULL);
//////////////////////////////////////////////////////////////////////////
//static AVOutputFormat* find_mp4_format() {
//    AVOutputFormat *fmt = NULL;
//    while ((fmt = av_oformat_next(fmt))) {
//      if (strcmp(fmt->extensions, "mp4") == 0)
//        return fmt;
//    }
//    return NULL;
//}

class InternalFFMpegRegister
{
public:
    InternalFFMpegRegister()
    {
        if (!initialized)
        {
#if DUMP_DEBUG_INFO
            LOGI("FFMPEG version: %d", avformat_version());
            LOGI("FFMPEG configuration: %s", avformat_configuration());
            av_log_set_level(AV_LOG_DEBUG);
#else
            av_log_set_level(AV_LOG_ERROR);
#endif

            avformat_network_init();

            /* register all codecs, demux and protocols */
            av_register_all();

            /* register a callback function for synchronization */
            //av_lockmgr_register(&LockCallBack);

            // NOTE: dump debug information
#if DUMP_DEBUG_INFO
            dump_all_iformat();
            dump_all_oformat();
#endif // DUMP_DEBUG_INFO

            initialized = true;
        }
    }
    ~InternalFFMpegRegister()
    {
        avformat_network_deinit();
        //av_lockmgr_register(NULL);

        initialized = false;
    }
    static bool initialized;
};

bool InternalFFMpegRegister::initialized = false;
static InternalFFMpegRegister s_init;

static const double EPS_ZERO = 0.000025;

static inline const char* err2str(int err)
{
    const int MSG_SZ = 1024;
    static char msg[MSG_SZ];
    av_strerror(err, msg, MSG_SZ);
    return msg;
}

static inline double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}

#define LOG_ERR(fmt, ...) LOGE(fmt ", %s (%d)", ##__VA_ARGS__, __FILE__, __LINE__)
#define LOG_ERR_R(err, fmt, ...) LOG_ERR(fmt ", AVERROR: %s", ##__VA_ARGS__, err2str(err))

void VideoCapture_FFMPEG::init()
{
    ENTER_FUNCTION;

    format_ctx = NULL;
    video_stream = -1;
    video_st = NULL;
    vcodec_ctx = NULL;
    src_frame = NULL;

    memset(&packet, 0, sizeof(packet));
    av_init_packet(&packet);

    dst_pix_fmt = AV_PIX_FMT_NV21;
    dst_width = dst_height = 0;
    dst_frame = NULL;

    img_convert_ctx = NULL;

    first_frame_number = -1;
    frame_number = 0;
    frame_count = 0;
    picture_pts = AV_NOPTS_VALUE;

    filename = 0;

    EXIT_FUNCTION;
}

void VideoCapture_FFMPEG::destory()
{
    ENTER_FUNCTION;

    if (img_convert_ctx)
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }

    if (src_frame)
    {
        av_frame_free(&src_frame);
        src_frame = NULL;
    }

    if (dst_frame)
    {
        if (dst_frame->data[0])
            avpicture_free((AVPicture *)dst_frame);
        av_frame_free(&dst_frame);
        dst_frame = NULL;
    }

    if (vcodec_ctx)
    {
        avcodec_close(vcodec_ctx);
        video_st = NULL;
        vcodec_ctx = NULL;
    }

    if (format_ctx)
    {
        avformat_close_input(&format_ctx);
        format_ctx = NULL;
    }

    if (packet.data)
    {
        av_free_packet(&packet);
        packet.data = NULL;
    }

    EXIT_FUNCTION;
}

void VideoCapture_FFMPEG::close()
{
    ENTER_FUNCTION;

    destory();
    init();

    EXIT_FUNCTION;
}

bool VideoCapture_FFMPEG::open(const char* fname)
{
    close();

    int err = avformat_open_input(&format_ctx, fname, NULL, NULL);
    if (err < 0)
    {
        LOG_ERR_R(err, "Error opening file %s", fname);
        return false;
    }
    err = avformat_find_stream_info(format_ctx, NULL);
    if (err < 0)
    {
        LOG_ERR_R(err, "Could not find codec parameters");
        return false;
    }
    // Dump information about file onto standard error
#if DUMP_DEBUG_INFO
    av_dump_format(format_ctx, 0, fname, 0);
#endif // DUMP_DEBUG_INFO
    for (unsigned i = 0; i < format_ctx->nb_streams; i++)
    {
        AVCodecContext *codec_ctx = format_ctx->streams[i]->codec;
        // Note: number of detected logical cores by default.
        // codec_ctx->thread_count = av_cpu_count();

        // get the video stream id from format context
        if (AVMEDIA_TYPE_VIDEO == codec_ctx->codec_type && video_stream < 0)
        {
            // backup encoder' width/height
            int enc_width = codec_ctx->width;
            int enc_height = codec_ctx->height;

            AVCodec *codec = avcodec_find_decoder(codec_ctx->codec_id);
            if (!codec)
            {
                LOG_ERR("Unsupported codec %s(%d)", codec_ctx->codec->name, codec_ctx->codec_id);
                return false;
            }
            int ret = avcodec_open2(codec_ctx, codec, NULL);
            if (ret < 0)
            {
              LOG_ERR_R(ret, "avcodec_open2 failed with codec %s(%d)", codec_ctx->codec->name, codec_ctx->codec_id);
              return false;
            }

            // checking width/height (since decoder can sometimes alter it, eg. vp6f)
            if (enc_width && (codec_ctx->width != enc_width))
              codec_ctx->width = enc_width;
            if (enc_height && (codec_ctx->height != enc_height))
              codec_ctx->height = enc_height;

            video_stream = i;
            video_st = format_ctx->streams[i];
            vcodec_ctx = video_st->codec;

            src_frame = av_frame_alloc();
            if (!src_frame)
            {
                LOG_ERR("Memory error");
              return false;
            }

            if (!set_target_picture(vcodec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height))
                return false;

            break;
        }
    }

    if (video_stream < 0)
        return false;

    // save some properties
    frame_count = get_total_frames();

    return true;
}

bool VideoCapture_FFMPEG::grabFrame()
{
    ENTER_FUNCTION;

    bool valid = false;
    int got_picture;

    int count_errs = 0;
    const int max_number_of_attempts = 100;

    if (!format_ctx || !vcodec_ctx)  return false;

    if (video_st->nb_frames > 0 && frame_number > video_st->nb_frames)
    {
        av_log(NULL, AV_LOG_DEBUG, "reach the end of the video");
        return false;
    }

    av_free_packet(&packet);
    picture_pts = AV_NOPTS_VALUE;

    // get the next frame
    while (!valid)
    {
        int ret = av_read_frame(format_ctx, &packet);
        if (ret == AVERROR(EAGAIN))
        {
            av_log(NULL, AV_LOG_DEBUG, "av_read_frame return: %s (%d)\n", err2str(ret), ret);
            continue;
        }
        else if (ret < 0 && ret != AVERROR_EOF)
        {
            av_log(NULL, AV_LOG_DEBUG, "av_read_frame return: %s (%d)\n", err2str(ret), ret);
        }

        // Is this a packet from the video stream?
        if (packet.stream_index != video_stream)
        {
            av_free_packet(&packet);
            count_errs++;
            if (count_errs > max_number_of_attempts)
            {
                LOG_ERR("can not grab frame (#%ld) with no video stream", (long) frame_number);
                break;
            }
            continue;
        }

        // Decode video frame
        ret = avcodec_decode_video2(vcodec_ctx, src_frame, &got_picture, &packet);
        if (ret < 0)
            return LOG_ERR_R(ret, "avcodec_decode_video2 failed (#%ld)", (long) frame_number), false;

        // Did we get a video frame?
        if (got_picture)
        {
            //picture_pts = picture->best_effort_timestamp;
            if (picture_pts == AV_NOPTS_VALUE)
                picture_pts = packet.pts != AV_NOPTS_VALUE && packet.pts != 0 ? packet.pts : packet.dts;
            frame_number++;
            valid = true;
        }
        else
        {
            count_errs++;
            if (count_errs > max_number_of_attempts)
            {
                LOG_ERR("can not grab frame (#%ld) with no picture", (long) frame_number);
                break;
            }
        }

        av_free_packet(&packet);
    }

    if (valid && first_frame_number < 0)
        first_frame_number = dts_to_frame_number(picture_pts);

    EXIT_FUNCTION;

    // return if we have a new picture or not
    return valid;
}

bool VideoCapture_FFMPEG::retrieveFrame(const uint8_t* data[4], int step[4])
{
    ENTER_FUNCTION;

    if (!src_frame)
        return false;

    if (dst_frame)
    {
        if (!dst_frame->data[0])
        {
            // create conversion context
            img_convert_ctx = sws_getCachedContext(
                img_convert_ctx,
                vcodec_ctx->width,
                vcodec_ctx->height,
                vcodec_ctx->pix_fmt,
                dst_width,
                dst_height,
                dst_pix_fmt,
                SWS_BICUBIC,
                NULL, NULL, NULL
                );
            if (!img_convert_ctx)
                return LOG_ERR("Cannot initialize the conversion context!"), false;

            int err = avpicture_alloc((AVPicture*)dst_frame, dst_pix_fmt, dst_width, dst_height);
            if (err < 0)
                return LOG_ERR_R(err, "avpicture_alloc"), false;
        }
        sws_scale(
            img_convert_ctx,
            src_frame->data,
            src_frame->linesize,
            0,
            src_frame->height,
            dst_frame->data,
            dst_frame->linesize
            );
    }

    AVFrame* frame = dst_frame ? dst_frame : src_frame;
    if (data)
        memcpy(data, frame->data, 4*sizeof(uint8_t *));
    if (step)
        memcpy(step, frame->linesize, 4*sizeof(int *));

    EXIT_FUNCTION;

    return true;
}

const uint8_t* VideoCapture_FFMPEG::retrieveFrame()
{
    const uint8_t* data[4];
    if (retrieveFrame(data, NULL))
        return data[0];
    else
        return NULL;
}

double VideoCapture_FFMPEG::getProperty(int property_id)
{
    if (!video_st) return 0;

    switch(property_id)
    {
    case FFMPEG_PROP_POS_MSEC:
        return 1000.0*frame_number/get_fps();
    case FFMPEG_PROP_POS_FRAMES:
        return frame_number;
    case FFMPEG_PROP_POS_AVI_RATIO:
        return r2d(video_st->time_base);
    case FFMPEG_PROP_FRAME_COUNT:
        return frame_count;
    case FFMPEG_PROP_FRAME_WIDTH:
        return dst_width;
    case FFMPEG_PROP_FRAME_HEIGHT:
        return dst_height;
    case FFMPEG_PROP_ORIGINAL_FRAME_WIDTH:
        return vcodec_ctx->width;
    case FFMPEG_PROP_ORIGINAL_FRAME_HEIGHT:
        return vcodec_ctx->height;
    case FFMPEG_PROP_FPS:
        return r2d(video_st->r_frame_rate);
    case FFMPEG_PROP_FOURCC:
        return vcodec_ctx->codec_tag;
    case FFMPEG_PROP_PIXEL_FORMAT:
        return (int) dst_pix_fmt;
    case FFMPEG_PROP_BUFFER_SIZE:
        return buf_size;
    default:
        break;
    }

    return 0;
}

bool VideoCapture_FFMPEG::setProperty(int property_id, double value)
{
    if (!video_st) return false;

    switch(property_id)
    {
    case FFMPEG_PROP_POS_MSEC:
    case FFMPEG_PROP_POS_FRAMES:
    case FFMPEG_PROP_POS_AVI_RATIO:
        switch(property_id)
        {
        case FFMPEG_PROP_POS_FRAMES:
            seek((int64_t)value);
            break;

        case FFMPEG_PROP_POS_MSEC:
            seek(value/1000.0);
            break;

        case FFMPEG_PROP_POS_AVI_RATIO:
            seek((int64_t)(value*format_ctx->duration));
            break;
        }
        picture_pts = (int64_t)value;
        break;

    case FFMPEG_PROP_FRAME_WIDTH:
        return set_target_picture(dst_pix_fmt, (int)value, dst_height);

    case FFMPEG_PROP_FRAME_HEIGHT:
        return set_target_picture(dst_pix_fmt, dst_width, (int)value);

    case FFMPEG_PROP_PIXEL_FORMAT:
        return set_target_picture((AVPixelFormat) (int) value, dst_width, dst_height);

    default:
        return false;
    }

    return true;
}

void VideoCapture_FFMPEG::seek(int64_t pos)
{
    pos = min(pos, frame_count);
    int delta = 16;

    // if we have not grabbed a single frame before first seek, let's read the first frame
    // and get some valuable information during the process
    if (first_frame_number < 0 && frame_count > 1)
        grabFrame();

    for (;;)
    {
        int64_t _frame_number_temp = max(pos-delta, (int64_t)0);
        double sec = (double)_frame_number_temp / get_fps();
        int64_t time_stamp = video_st->start_time;
        double  time_base  = r2d(video_st->time_base);
        time_stamp += (int64_t)(sec / time_base + 0.5);
        if (frame_count > 1)
            av_seek_frame(format_ctx, video_stream, time_stamp, AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(vcodec_ctx);
        if (pos > 0)
        {
            grabFrame();

            if (pos > 1)
            {
                frame_number = dts_to_frame_number(picture_pts) - first_frame_number;
                //LOGD("_frame_number = %d, frame_number = %d, delta = %d\n",
                //       (int)_frame_number, (int)frame_number, delta);

                if (frame_number < 0 || frame_number > pos-1)
                {
                    if (_frame_number_temp == 0 || delta >= INT_MAX/4)
                        break;
                    delta = delta < 16 ? delta*2 : delta*3/2;
                    continue;
                }
                while (frame_number < pos-1)
                {
                    if (!grabFrame())
                        break;
                }
                frame_number++;
                break;
            }
            else
            {
                frame_number = 1;
                break;
            }
        }
        else
        {
            frame_number = 0;
            break;
        }
    }
}

void VideoCapture_FFMPEG::seek(double sec)
{
    seek((int64_t)(sec * get_fps() + 0.5));
}

double VideoCapture_FFMPEG::get_duration_sec()
{
    double sec = (double)format_ctx->duration / (double)AV_TIME_BASE;

    if (sec < EPS_ZERO)
    {
        sec = (double)video_st->duration * r2d(video_st->time_base);
    }

    return sec;
}

int VideoCapture_FFMPEG::get_bitrate()
{
    return format_ctx->bit_rate;
}

double VideoCapture_FFMPEG::get_fps()
{
    double fps = r2d(video_st->r_frame_rate);
    if (fps < EPS_ZERO)
        fps = r2d(video_st->avg_frame_rate);

    return fps;
}

int64_t VideoCapture_FFMPEG::get_total_frames()
{
    int64_t nbf = video_st->nb_frames;

    if (nbf == 0)
    {
        LOGW("video_st->nb_frames is zero, calculate total frames from fps (%.3f) and duration (%.3f)",
             get_fps(), get_duration_sec());
        nbf = (int64_t) floor(get_duration_sec() * get_fps() + 0.5);
    }

    return nbf;
}

int64_t VideoCapture_FFMPEG::dts_to_frame_number(int64_t dts)
{
    double sec = dts_to_sec(dts);
    return (int64_t)(get_fps() * sec + 0.5);
}

double VideoCapture_FFMPEG::dts_to_sec(int64_t dts)
{
    return (double)(dts - video_st->start_time) *
        r2d(video_st->time_base);
}

bool VideoCapture_FFMPEG::set_target_picture(AVPixelFormat fmt, int width, int height)
{
    ENTER_FUNCTION;

    // check parameters
    if (fmt == AV_PIX_FMT_NONE || width <= 0 || height <= 0)
    {
        LOG_ERR("Invalid parameters");
        return false;
    }

    if (width == dst_width && height == dst_height && dst_pix_fmt == fmt)
        return true;

    bool needcvt = fmt != vcodec_ctx->pix_fmt || width != vcodec_ctx->width || height != vcodec_ctx->height;
    if (needcvt)
    {
        if (!dst_frame)
        {
            dst_frame = av_frame_alloc();
            if (!dst_frame)
            {
                LOG_ERR("Memory error");
                return false;
            }
        }
    }
    if (dst_frame && dst_frame->data[0])
        avpicture_free((AVPicture *)dst_frame);
    if (!needcvt)
    {
        av_frame_free(&dst_frame);
        dst_frame = NULL;
    }

    dst_pix_fmt = fmt;
    dst_width = width;
    dst_height = height;
    buf_size = avpicture_get_size(fmt, width, height);

    EXIT_FUNCTION;

    return true;
}

// AVFormatContext (s)
//  - AVStream (st)
//      - AVCodecContext (codec)
//          - codec_tag?
//          - codec_id?
//  - AVOutputFormat (of)

/* add a video output stream to the container */
static AVStream *add_video_stream(AVFormatContext *oc,
                                  AVCodecID codec_id,
                                  int w, int h, int bitrate,
                                  double fps, AVPixelFormat pixel_format)
{
    AVCodecContext *c;
    AVStream *st;
    int frame_rate, frame_rate_base;

    st = avformat_new_stream(oc, 0);
    if (!st)
    {
        LOG_ERR("Could not allocate stream");
        return NULL;
    }

    c = st->codec;
    if (codec_id != AV_CODEC_ID_NONE)
        c->codec_id = codec_id;
    else
        c->codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, AVMEDIA_TYPE_VIDEO);

    AVCodec *codec = avcodec_find_encoder(c->codec_id);
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    /* put sample parameters */
    int64_t lbit_rate = (int64_t)bitrate;
    lbit_rate += (bitrate / 2);
    lbit_rate = min(lbit_rate, (int64_t)INT_MAX);
    c->bit_rate = lbit_rate;

    // took advice from
    // http://ffmpeg-users.933282.n4.nabble.com/warning-clipping-1-dct-coefficients-to-127-127-td934297.html
    c->qmin = 3;

    /* resolution must be a multiple of two */
    c->width = w;
    c->height = h;

    /* time base: this is the fundamental unit of time (in seconds) in terms
       of which frame timestamps are represented. for fixed-fps content,
       timebase should be 1/framerate and timestamp increments should be
       identically 1. */
    frame_rate=(int)(fps+0.5);
    frame_rate_base=1;
    while (fabs((double)frame_rate/frame_rate_base) - fps > 0.001)
    {
        frame_rate_base*=10;
        frame_rate=(int)(fps*frame_rate_base + 0.5);
    }
    c->time_base.den = frame_rate;
    c->time_base.num = frame_rate_base;
    /* adjust time base for supported framerates */
    if (codec && codec->supported_framerates)
    {
        const AVRational *p= codec->supported_framerates;
        AVRational req = {frame_rate, frame_rate_base};
        const AVRational *best=NULL;
        AVRational best_error= {INT_MAX, 1};
        for (; p->den!=0; p++)
        {
            AVRational error= av_sub_q(req, *p);
            if (error.num <0) error.num *= -1;
            if (av_cmp_q(error, best_error) < 0)
            {
                best_error= error;
                best= p;
            }
        }
        c->time_base.den= best->num;
        c->time_base.num= best->den;
    }

    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = pixel_format;

    if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
        c->max_b_frames = 2;
    if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO || c->codec_id == AV_CODEC_ID_MSMPEG4V3)
    {
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        /* avoid FFMPEG warning 'clipping 1 dct coefficients...' */
        c->mb_decision=2;
    }
    // some formats want stream headers to be seperate
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

static int NO_FRAMES_WRITTEN_CODE = 1000;
static int write_frame(AVFormatContext* oc, AVStream* video_st, AVFrame* picture)
{
    int ret = 0;
    AVPacket pkt;

    if (oc->oformat->flags & AVFMT_RAWPICTURE)
    {
        /* raw video case. The API will change slightly in the near future for that */
        av_init_packet(&pkt);
        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= video_st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);
        ret = av_write_frame(oc, &pkt);
        if (ret < 0) LOG_ERR_R(ret, "av_write_frame");
    }
    else
    {
        /* encode the image */
        AVCodecContext *c = video_st->codec;
        int got_packet;

        // The encoder will allocate the output buffer, free it by av_free_packet()
        pkt.data = NULL;
        pkt.size = 0;

        av_init_packet(&pkt);

        /* encode the image */
        ret = avcodec_encode_video2(c, &pkt, picture, &got_packet);
        if (ret < 0) LOG_ERR_R(ret, "avcodec_encode_video2");

        /* if zero size, it means the image was buffered */
        if (ret == 0 && got_packet > 0)
        {
            if (pkt.pts != (int64_t) AV_NOPTS_VALUE)
                pkt.pts = av_rescale_q(pkt.pts, c->time_base, video_st->time_base);
            if (pkt.dts != (int64_t) AV_NOPTS_VALUE)
                pkt.dts = av_rescale_q(pkt.dts, c->time_base, video_st->time_base);
            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
        }
        else
        {
            ret = NO_FRAMES_WRITTEN_CODE;
        }

        av_free_packet(&pkt);
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////

void VideoWriter_FFMPEG::init()
{
    oformat = 0;
    format_ctx = NULL;
    dst_picture = NULL;
    picbuf = NULL;
    video_st = NULL;
    img_convert_ctx = NULL;

    input_pix_fmt = AV_PIX_FMT_NONE;
    frame_width = frame_height = 0;
    input_picture = NULL;

    ok = false;
}

void VideoWriter_FFMPEG::destroy()
{
    // nothing to do if already released
    if (!dst_picture)
        return;

    /* no more frame to compress. The codec has a latency of a few
       frames if using B frames, so we get the last frames by
       passing the same picture again */
    // TODO -- do we need to account for latency here?

    /* write the trailer, if any */
    if (ok && format_ctx)
    {
        if ((format_ctx->oformat->flags & AVFMT_RAWPICTURE) == 0)
        {
            for (;;)
            {
                int ret = write_frame(format_ctx, video_st, NULL);
                if (ret == NO_FRAMES_WRITTEN_CODE || ret < 0)
                    break;
            }
        }
        int err = av_write_trailer(format_ctx);
        if (err < 0) LOG_ERR_R(err, "av_write_trailer");
    }

    if (img_convert_ctx)
    {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }

    // free pictures
    if (video_st->codec->pix_fmt != input_pix_fmt)
        avpicture_free((AVPicture *)dst_picture);
    av_free(dst_picture);

    if (input_picture)
        av_free(input_picture);

    /* close codec */
    avcodec_close(video_st->codec);

    /* free the streams */
    //for (unsigned i = 0; i < format_ctx->nb_streams; i++)
    //{
    //    av_freep(&format_ctx->streams[i]->codec);
    //    av_freep(&format_ctx->streams[i]);
    //}

    /* close the output file */
    if (!(oformat->flags & AVFMT_NOFILE))
    {
        avio_close(format_ctx->pb);
    }

    /* free the stream */
    avformat_free_context(format_ctx);
}

/// close video output stream and free associated memory
void VideoWriter_FFMPEG::close()
{
    destroy();
    init();
}

/// Create a video writer object that uses FFMPEG
bool VideoWriter_FFMPEG::open(const char* filename, unsigned fourcc, double fps,
                              int width, int height, AVPixelFormat src_pix_fmt)
{
    int err;

    close();

    // check arguments
    if (!filename || fps < 0)
    {
        LOG_ERR("Invalid file name or fps");
        return false;
    }

    // we allow frames of odd width or height, but in this case we truncate
    // the rightmost column/the bottom row. Probably, this should be handled more elegantly,
    // but some internal functions inside FFMPEG swscale require even width/height.
    width &= ~1;
    height &= ~1;
    if (width <= 0 || height <= 0)
    {
        LOG_ERR("Invalid width or height");
        return false;
    }

    // auto detect the output format from file name and fourcc code.
    oformat = av_guess_format(NULL, filename, NULL);
    //oformat = find_mp4_format();
    if (!oformat)
    {
        LOGW("Could not deduce output format from file extension (%s): using MPEG.", filename);
        oformat = av_guess_format("mpeg", NULL, NULL);
        if (!oformat)
        {
            LOG_ERR("Could not deduce output format from short name (mpeg)");
            return false;
        }
    }
    av_log(NULL, AV_LOG_DEBUG, "guess output format (%s) by file name\n", oformat->name);
    av_log(NULL, AV_LOG_DEBUG, "    default video codec: (%s/%d)\n",
           avcodec_get_name(oformat->video_codec), oformat->video_codec);

    // Lookup codec_id for given fourcc
    AVCodecID codec_id = oformat->video_codec;
    if (fourcc > 0)
    {
#if 1
        const struct AVCodecTag *tags[] = { avformat_get_riff_video_tags(), 0 };
        codec_id = av_codec_get_id(tags, fourcc);
#else
        codec_id = av_codec_get_id(oformat->codec_tag, fourcc);
#endif
        av_log(NULL, AV_LOG_DEBUG, "get codec (%s/%d) from FOURCC (0x%08x)\n",
               avcodec_get_name(codec_id), codec_id, fourcc);
    }
    else
    {
#if 0
        const struct AVCodecTag *tags[] = { avformat_get_riff_video_tags(), 0 };
        fourcc = av_codec_get_tag(tags, codec_id);
#else
        fourcc = av_codec_get_tag(oformat->codec_tag, codec_id);
#endif
        av_log(NULL, AV_LOG_DEBUG, "get FOURCC (0x%08x) from codec id (%s/%d)\n",
               fourcc, avcodec_get_name(codec_id), codec_id);
    }
    if (codec_id == AV_CODEC_ID_NONE)
    {
        LOG_ERR("No codec id is found!");
        return false;
    }

    // alloc memory for context
    format_ctx = avformat_alloc_context();
    if (!format_ctx)
    {
        LOG_ERR("Memory error");
        return false;
    }

    /* set file name */
    format_ctx->oformat = oformat;
    snprintf(format_ctx->filename, sizeof(format_ctx->filename), "%s", filename);

    /* set some options */
    format_ctx->max_delay = (int)(0.7*AV_TIME_BASE);  /* This reduces buffer under-run warnings with MPEG */

    // set a few optimal pixel formats for lossless codecs of interest..
    input_pix_fmt = src_pix_fmt;
    AVPixelFormat codec_pix_fmt;
    double bitrate_scale = 1;
    switch (codec_id)
    {
    case AV_CODEC_ID_JPEGLS:
        // BGR24 or GRAY8 depending on is_color...
        codec_pix_fmt = input_pix_fmt;
        break;
    case AV_CODEC_ID_HUFFYUV:
        codec_pix_fmt = AV_PIX_FMT_YUV422P;
        break;
    case AV_CODEC_ID_MJPEG:
    case AV_CODEC_ID_LJPEG:
        codec_pix_fmt = AV_PIX_FMT_YUVJ420P;
        bitrate_scale = 3;
        break;
    case AV_CODEC_ID_RAWVIDEO:
        codec_pix_fmt = input_pix_fmt == AV_PIX_FMT_GRAY8 ||
            input_pix_fmt == AV_PIX_FMT_GRAY16LE ||
            input_pix_fmt == AV_PIX_FMT_GRAY16BE ? input_pix_fmt : AV_PIX_FMT_YUV420P;
        break;
    default:
        // good for lossy formats, MPEG, etc.
        codec_pix_fmt = AV_PIX_FMT_YUV420P;
        break;
    }
    av_log(NULL, AV_LOG_DEBUG, "input pixel format: %s (%d)\n",
           av_get_pix_fmt_name(input_pix_fmt), input_pix_fmt);
    av_log(NULL, AV_LOG_DEBUG, "codec pixel format: %s (%d)\n",
           av_get_pix_fmt_name(codec_pix_fmt), codec_pix_fmt);
    double bitrate = min(bitrate_scale*fps*width*height, (double)INT_MAX/2);

    // TODO -- safe to ignore output audio stream?
    video_st = add_video_stream(format_ctx, codec_id, width, height, (int)(bitrate + 0.5), fps, codec_pix_fmt);

#if 0 // for debug
    av_dump_format(format_ctx, 0, filename, 1);
#endif

    /* now that all the parameters are set, we can open the audio and
     video codecs and allocate the necessary encode buffers */
    if (!video_st)
    {
        LOG_ERR("Failed to add video stream");
        return false;
    }

    AVCodec *codec;
    AVCodecContext *c;

    c = video_st->codec;
    c->codec_tag = fourcc;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec)
    {
        LOG_ERR("Could not find encoder for codec id %s (%d)", avcodec_get_name(c->codec_id), c->codec_id);
        return false;
    }

    int64_t lbit_rate = (int64_t)c->bit_rate;
    lbit_rate += (bitrate / 2);
    lbit_rate = min(lbit_rate, (int64_t)INT_MAX);
    c->bit_rate_tolerance = (int)lbit_rate;
    c->bit_rate = (int)lbit_rate;

    /* open the codec */
    if ((err = avcodec_open2(c, codec, NULL)) < 0)
    {
        LOG_ERR_R(err, "Could not open codec '%s'", codec->name);
        return false;
    }

    /* allocate the encoded raw dst_picture */
    dst_picture = av_frame_alloc();
    if (!dst_picture)
    {
        LOG_ERR("Memory error");
        return false;
    }
    dst_picture->format = c->pix_fmt;
    dst_picture->width = c->width;
    dst_picture->height = c->height;
    if (c->pix_fmt != input_pix_fmt)
    {
        /* if the output format is not our input format, then a temporary
        picture of the input format is needed too. It is then converted
        to the required output format */
        av_log(NULL, AV_LOG_DEBUG, "Input pixel format (%s) is not same with output format (%s)\n",
               av_get_pix_fmt_name(input_pix_fmt), av_get_pix_fmt_name(c->pix_fmt));
        avpicture_alloc((AVPicture *)dst_picture, c->pix_fmt, c->width, c->height);
        input_picture = av_frame_alloc();
        if (!input_picture)
        {
            LOG_ERR("Memory error");
            return false;
        }
    }

    /* open the output file, if needed */
    if (!(oformat->flags & AVFMT_NOFILE))
    {
        if ((err = avio_open(&format_ctx->pb, filename, AVIO_FLAG_WRITE)) < 0)
        {
            LOG_ERR_R(err, "Failed to open output video file");
            return false;
        }
    }

    /* write the stream header, if any */
    if ((err = avformat_write_header(format_ctx, NULL)) < 0)
    {
        LOG_ERR_R(err, "Failed to write video header");
        close();
        if (!(oformat->flags & AVFMT_NOFILE))
            remove(filename);
        return false;
    }
    frame_width = width;
    frame_height = height;
    ok = true;

    return true;
}

/// write a frame with FFMPEG
bool VideoWriter_FFMPEG::writeFrame(const uint8_t* data)
{
    // typecast from opaque data type to implemented struct
    AVCodecContext *c = video_st->codec;
    if (input_picture)
    {
        // let input_picture point to the raw data buffer of 'image'
        avpicture_fill((AVPicture *)input_picture, (uint8_t *) data,
                       input_pix_fmt, frame_width, frame_height);

        if (!img_convert_ctx)
        {
            av_log(NULL, AV_LOG_DEBUG, "Create conversion context: (%s:%dx%d)->(%s:%dx%d)\n",
                   av_get_pix_fmt_name(input_pix_fmt), frame_width, frame_height,
                   av_get_pix_fmt_name(c->pix_fmt), c->width, c->height);
            img_convert_ctx = sws_getContext(frame_width,
                                             frame_height,
                                             input_pix_fmt,
                                             c->width,
                                             c->height,
                                             c->pix_fmt,
                                             SWS_BICUBIC,
                                             NULL, NULL, NULL);
            if (!img_convert_ctx)
                return false;
        }

        if (sws_scale(img_convert_ctx, input_picture->data,
                      input_picture->linesize, 0, frame_height,
                      dst_picture->data, dst_picture->linesize) < 0)
            return false;
    }
    else
    {
        avpicture_fill((AVPicture *)dst_picture, data,
                       input_pix_fmt, frame_width, frame_height);
    }

    //av_log(NULL, AV_LOG_DEBUG, "dst_picture: %d, %dx%d\n", dst_picture->format, dst_picture->width, dst_picture->height);
    int ret = write_frame(format_ctx, video_st, dst_picture);

    return ret >= 0;
}

// vim: ts=4 sts=4 sw=4 et
