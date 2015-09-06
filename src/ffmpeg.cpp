#include "ffmpeg.hpp"

#define NO_FUNC_LOG
#define LOG_TAG "ffmpeg"
#include "common/log.hpp"

#include <math.h>       /* floor */

class InternalFFMpegRegister
{
public:
    InternalFFMpegRegister() {
        if (!initialized) {
            //avformat_network_init();

            /* register all codecs, demux and protocols */
            av_register_all();

            /* register a callback function for synchronization */
            //av_lockmgr_register(&LockCallBack);

            av_log_set_level(AV_LOG_ERROR);

            initialized = true;
        }
    }
    ~InternalFFMpegRegister() {
        initialized = false;
        //av_lockmgr_register(NULL);
    }
    static bool initialized;
};

bool InternalFFMpegRegister::initialized = false;
static InternalFFMpegRegister s_init;
static const double EPS_ZERO = 0.000025;

static const char* err2str(int err) {
    const int MSG_SZ = 1024;
    static char msg[MSG_SZ];
    av_strerror(err, msg, MSG_SZ);
    return msg;
}
static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

#define LOG_ERR(err) { if (err != 0) LOGE("AVERROR: %s", err2str(err)); }

void VideoCapture_FFMPEG::init()
{
    ENTER_FUNCTION;

    format_ctx = NULL;
    video_stream = -1;
    video_st = NULL;
    vcodec_ctx = NULL;
    src_frame = NULL;

    dst_pix_fmt = AV_PIX_FMT_BGR24;
    dst_width = dst_height = 0;
    dst_frame = NULL;

    img_convert_ctx = NULL;

    first_frame_number = -1;
    frame_number = 0;
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
        avcodec_free_frame(&src_frame);
        src_frame = NULL;
    }

    if (dst_frame)
    {
        if (dst_frame->data[0])
            avpicture_free((AVPicture *)dst_frame);
        avcodec_free_frame(&dst_frame);
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
    // NOTE: for gdb
#if 0
    int i = 0;
    int a = 0;
    while (!i) {
        a++;
    }
#endif

    close();

    int err = avformat_open_input(&format_ctx, fname, NULL, NULL);
    if (err < 0)
    {
        LOGE("Error opening file %s", fname);
        LOG_ERR(err);
        return false;
    }
    err = avformat_find_stream_info(format_ctx, NULL);
    if (err < 0)
    {
        LOGE("Could not find codec parameters");
        LOG_ERR(err);
        return false;
    }
    // Dump information about file onto standard error
    av_dump_format(format_ctx, 0, fname, 0);
    for (unsigned i = 0; i < format_ctx->nb_streams; i++)
    {
    	AVCodecContext *codec_ctx = format_ctx->streams[i]->codec;
        //enc->thread_count = get_number_of_cpus();
        //codec_ctx->thread_count = 1;

        // get the video stream id from format context
        if (AVMEDIA_TYPE_VIDEO == codec_ctx->codec_type && video_stream < 0)
        {
            // backup encoder' width/height
            int enc_width = codec_ctx->width;
            int enc_height = codec_ctx->height;

            AVCodec *codec = avcodec_find_decoder(codec_ctx->codec_id);
            if (!codec || avcodec_open2(codec_ctx, codec, NULL) < 0)
            {
                LOGE("Unsupported codec %s(%d)", codec_ctx->codec_name, codec_ctx->codec_id);
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

            src_frame = avcodec_alloc_frame();
            if (!src_frame || !set_target_picture(dst_pix_fmt, codec_ctx->width, codec_ctx->height))
                return false;

            break;
        }
    }

    return video_stream >= 0;
}

bool VideoCapture_FFMPEG::grabFrame()
{
    ENTER_FUNCTION;

    bool valid = false;
    int got_picture;

    int count_errs = 0;
    const int max_number_of_attempts = 1 << 16;

    if (!format_ctx || !vcodec_ctx)  return false;

    //LOGW("%d/%d", (int)frame_number, (int)video_st->nb_frames);

    if (video_st->nb_frames > 0 && frame_number > video_st->nb_frames)
    {
        LOGE("Reach the end of the video");
        return false;
    }

    picture_pts = AV_NOPTS_VALUE;

    // get the next frame
    AVPacket packet;
    av_init_packet(&packet);
    while (av_read_frame(format_ctx, &packet) >= 0)
    {
        // Is this a packet from the video stream?
        if (packet.stream_index == video_stream)
        {
            // Decode video frame
            if (avcodec_decode_video2(vcodec_ctx, src_frame, &got_picture, &packet) > 0)
            {
                // Did we get a video frame?
                if (got_picture)
                {
                    //picture_pts = picture->best_effort_timestamp;
                    if (picture_pts == AV_NOPTS_VALUE)
                        picture_pts = packet.pts != AV_NOPTS_VALUE && packet.pts != 0 ? packet.pts : packet.dts;
                    frame_number++;
                    valid = true;
                    break;
                }
                else if (count_errs++ > max_number_of_attempts)
                {
                    LOGE("Can not grab frame (#%ld)", (long)frame_number);
                    av_free_packet(&packet);
                    break;
                }
            }
            else
            {
                LOGE("avcodec_decode_video2 failed (#%ld)", (long)frame_number);
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

    if (dst_frame) {
        if (!dst_frame->data[0]) {
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
            if (!img_convert_ctx) {
                LOGE("Cannot initialize the conversion context!");
                return false;
            }
            if (avpicture_alloc((AVPicture*)dst_frame, dst_pix_fmt, dst_width, dst_height) != 0) {
                LOGE("Memory error");
                return false;
            }
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

    if (data)
        memcpy(data, dst_frame ? dst_frame->data : src_frame->data, 4*sizeof(uint8_t *));
    if (step)
        memcpy(step, dst_frame ? dst_frame->linesize : src_frame->linesize, 4*sizeof(int *));

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
        return get_total_frames();
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
    pos = min(pos, get_total_frames());
    int delta = 16;

    // if we have not grabbed a single frame before first seek, let's read the first frame
    // and get some valuable information during the process
    if (first_frame_number < 0 && get_total_frames() > 1)
        grabFrame();

    for (;;)
    {
        int64_t _frame_number_temp = max(pos-delta, (int64_t)0);
        double sec = (double)_frame_number_temp / get_fps();
        int64_t time_stamp = video_st->start_time;
        double  time_base  = r2d(video_st->time_base);
        time_stamp += (int64_t)(sec / time_base + 0.5);
        if (get_total_frames() > 1) av_seek_frame(format_ctx, video_stream, time_stamp, AVSEEK_FLAG_BACKWARD);
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
        nbf = (int64_t)floor(get_duration_sec() * get_fps() + 0.5);

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
    if (fmt == AV_PIX_FMT_NONE || width <= 0 || height <= 0) {
        LOGE("Invalid parameters");
        return false;
    }

    if (width == dst_width && height == dst_height && dst_pix_fmt == fmt)
        return true;

    bool needcvt = fmt != vcodec_ctx->pix_fmt || width != vcodec_ctx->width || height != vcodec_ctx->height;
    if (needcvt) {
        if (!dst_frame) {
            dst_frame = avcodec_alloc_frame();
            if (!dst_frame) {
                LOGE("Memory error");
                return false;
            }
        }
    }
    if (dst_frame && dst_frame->data[0])
        avpicture_free((AVPicture *)dst_frame);
    if (!needcvt) {
        avcodec_free_frame(&dst_frame);
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
                                  int w, int h, int bitrate,
                                  double fps, AVPixelFormat pixel_format)
{
    AVCodecContext *c;
    AVStream *st;
    int frame_rate, frame_rate_base;

    st = avformat_new_stream(oc, 0);
    if (!st) {
        LOGE("Could not allocate stream");
        return NULL;
    }

    c = st->codec;
    c->codec_id = av_guess_codec(oc->oformat, NULL, oc->filename, NULL, AVMEDIA_TYPE_VIDEO);

    AVCodec *codec = avcodec_find_encoder(c->codec_id);
    c->codec_type = AVMEDIA_TYPE_VIDEO;

    // get the codec tag for the given codec id.
    const struct AVCodecTag *tags[] = { avformat_get_riff_video_tags(), 0 };
    c->codec_tag = av_codec_get_tag(tags, c->codec_id);

    LOGD("Tag: %08x, codec id '%d'", c->codec_tag, c->codec_id);

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
    while (fabs((double)frame_rate/frame_rate_base) - fps > 0.001) {
        frame_rate_base*=10;
        frame_rate=(int)(fps*frame_rate_base + 0.5);
    }
    c->time_base.den = frame_rate;
    c->time_base.num = frame_rate_base;
    /* adjust time base for supported framerates */
    if (codec && codec->supported_framerates) {
        const AVRational *p= codec->supported_framerates;
        AVRational req = {frame_rate, frame_rate_base};
        const AVRational *best=NULL;
        AVRational best_error= {INT_MAX, 1};
        for (; p->den!=0; p++) {
            AVRational error= av_sub_q(req, *p);
            if (error.num <0) error.num *= -1;
            if (av_cmp_q(error, best_error) < 0) {
                best_error= error;
                best= p;
            }
        }
        c->time_base.den= best->num;
        c->time_base.num= best->den;
    }

    c->gop_size = 12; /* emit one intra frame every twelve frames at most */
    c->pix_fmt = pixel_format;

    if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
        c->max_b_frames = 2;
    }
    if (c->codec_id == CODEC_ID_MPEG1VIDEO || c->codec_id == CODEC_ID_MSMPEG4V3) {
        /* needed to avoid using macroblocks in which some coeffs overflow
           this doesnt happen with normal video, it just happens here as the
           motion of the chroma plane doesnt match the luma plane */
        /* avoid FFMPEG warning 'clipping 1 dct coefficients...' */
        c->mb_decision=2;
    }
    // some formats want stream headers to be seperate
    if (oc->oformat->flags & AVFMT_GLOBALHEADER) {
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    return st;
}
static int write_frame(AVFormatContext* oc, AVStream* video_st, AVFrame* picture)
{
    int ret = 0;
    AVPacket pkt;
    av_init_packet(&pkt);

    if (oc->oformat->flags & AVFMT_RAWPICTURE) {
        /* raw video case. The API will change slightly in the near future for that */
        pkt.flags |= AV_PKT_FLAG_KEY;
        pkt.stream_index= video_st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);
        ret = av_write_frame(oc, &pkt);
    } else {
        /* encode the image */
        AVCodecContext *c = video_st->codec;
        int got_packet;

        pkt.data = NULL;
        pkt.size = 0;
        ret = avcodec_encode_video2(c, &pkt, picture, &got_packet);
        /* if zero size, it means the image was buffered */
        if (ret == 0 && got_packet > 0) {
            if (pkt.pts != AV_NOPTS_VALUE)
                pkt.pts = av_rescale_q(pkt.pts, c->time_base, video_st->time_base);
            if (pkt.dts != AV_NOPTS_VALUE)
                pkt.dts = av_rescale_q(pkt.dts, c->time_base, video_st->time_base);
            /* write the compressed frame in the media file */
            ret = av_write_frame(oc, &pkt);
            av_free_packet(&pkt);
        }
    }
    return ret;
}

//////////////////////////////////////////////////////////////////////////

void VideoWriter_FFMPEG::init()
{
    oformat = 0;
    format_ctx = NULL;
    picture = NULL;
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
    if (!picture)
        return;

    /* no more frame to compress. The codec has a latency of a few
       frames if using B frames, so we get the last frames by
       passing the same picture again */
    // TODO -- do we need to account for latency here?

    /* write the trailer, if any */
    if (ok && format_ctx) {
        if ((format_ctx->oformat->flags & AVFMT_RAWPICTURE) == 0) {
            for (;;) {
                if (write_frame(format_ctx, video_st, NULL) < 0)
                    break;
            }
        }
        av_write_trailer(format_ctx);
    }

    if (img_convert_ctx) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }

    // free pictures
    if (video_st->codec->pix_fmt != input_pix_fmt)
        avpicture_free((AVPicture *)picture);
    av_free(picture);

    if (input_picture)
        av_free(input_picture);

    /* close codec */
    avcodec_close(video_st->codec);

    /* free the streams */
    for (unsigned i = 0; i < format_ctx->nb_streams; i++) {
        av_freep(&format_ctx->streams[i]->codec);
        av_freep(&format_ctx->streams[i]);
    }

    if (!(oformat->flags & AVFMT_NOFILE)) {
        /* close the output file */
        avio_close(format_ctx->pb);
    }

    /* free the stream */
    av_free(format_ctx);
}

/// close video output stream and free associated memory
void VideoWriter_FFMPEG::close()
{
    destroy();
    init();
}

/// Create a video writer object that uses FFMPEG
bool VideoWriter_FFMPEG::open(const char* filename, double fps, int width, int height, AVPixelFormat src_pix_fmt)
{
    int err;

    close();

    // check arguments
    if (!filename || fps < 0) {
        LOGE("Invalid file name or fps");
        return false;
    }

    // we allow frames of odd width or height, but in this case we truncate
    // the rightmost column/the bottom row. Probably, this should be handled more elegantly,
    // but some internal functions inside FFMPEG swscale require even width/height.
    width &= -2;
    height &= -2;
    if (width <= 0 || height <= 0) {
        LOGE("Invalid width or height");
        return false;
    }

    /* auto detect the output format from file name and fourcc code. */
    oformat = av_guess_format(NULL, filename, NULL);
    if (!oformat) {
        LOGW("Could not deduce output format from file extension (%s): using MPEG.", filename);
        oformat = av_guess_format("mpeg", NULL, NULL);
        if (!oformat) {
            LOGE("Could not deduce output format with short name (mpeg)");
            return false;
        }
    }

    // alloc memory for context
    format_ctx = avformat_alloc_context();
    if (!format_ctx) {
        LOGE("Memory error");
        return false;
    }

    /* set file name */
    format_ctx->oformat = oformat;
    snprintf(format_ctx->filename, sizeof(format_ctx->filename), "%s", filename);

    /* set some options */
    format_ctx->max_delay = (int)(0.7*AV_TIME_BASE);  /* This reduces buffer under-run warnings with MPEG */

    // set a few optimal pixel formats for lossless codecs of interest..
    input_pix_fmt = src_pix_fmt;
    // good for lossy formats, MPEG, etc.
    AVPixelFormat codec_pix_fmt = PIX_FMT_YUV420P;
    double bitrate = min(fps*width*height, (double)INT_MAX/2);

    // TODO -- safe to ignore output audio stream?
    video_st = add_video_stream(format_ctx, width, height, (int)(bitrate + 0.5), fps, codec_pix_fmt);
    
    /* set the output parameters (must be done even if no parameters). */
#if 0
    av_dump_format(oc, 0, filename, 1);
#endif

    /* now that all the parameters are set, we can open the audio and
     video codecs and allocate the necessary encode buffers */
    if (!video_st) {
        LOGE("Failed to add video stream");
        return false;
    }

    AVCodec *codec;
    AVCodecContext *c;

    c = (video_st->codec);
    //c->codec_tag = fourcc;

    /* find the video encoder */
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        LOGE("Could not find encoder for codec id %d", c->codec_id);
        return false;
    }

    int64_t lbit_rate = (int64_t)c->bit_rate;
    lbit_rate += (bitrate / 2);
    lbit_rate = min(lbit_rate, (int64_t)INT_MAX);
    c->bit_rate_tolerance = (int)lbit_rate;
    c->bit_rate = (int)lbit_rate;

    /* open the codec */
    if ((err = avcodec_open2(c, codec, NULL)) < 0) {
        LOGE("Could not open codec '%s'", codec->name);
        LOG_ERR(err);
        return false;
    }

    /* allocate the encoded raw picture */
    picture = avcodec_alloc_frame();
    if (!picture) {
        LOGE("Memory error");
        return false;
    }
    if (c->pix_fmt != input_pix_fmt) {
        /* if the output format is not our input format, then a temporary
        picture of the input format is needed too. It is then converted
        to the required output format */
        avpicture_alloc((AVPicture *)picture, c->pix_fmt, c->width, c->height);
        input_picture = avcodec_alloc_frame();
        if (!input_picture) {
            LOGE("Memory error");
            return false;
        }
    }

    /* open the output file, if needed */
    if (!(oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&format_ctx->pb, filename, AVIO_FLAG_WRITE) < 0) {
            LOGE("Failed to open output video file");
            return false;
        }
    }

    /* write the stream header, if any */
    err = avformat_write_header(format_ctx, NULL);
    if (err < 0) {
        close();
        remove(filename);
        LOGE("Failed to write video header");
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
    if (input_picture) {
        // let input_picture point to the raw data buffer of 'image'
        avpicture_fill((AVPicture *)input_picture, (uint8_t *) data,
                       input_pix_fmt, frame_width, frame_height);

        if (!img_convert_ctx)
        {
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
                      picture->data, picture->linesize) < 0)
            return false;
    } else {
        avpicture_fill((AVPicture *)picture, data,
                       input_pix_fmt, frame_width, frame_height);
    }

    int ret = write_frame(format_ctx, video_st, picture);
    LOG_ERR(ret);

    return ret >= 0;
}
