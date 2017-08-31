
/**
 * Created by jianxi on 2017/5/12.
 * https://github.com/mabeijianxi
 * mabeijianxi@gmail.com
 */
#include "jx_yuv_encode_h264.h"
#include "jx_jni_handler.h"
#include "jx_log.h"
#include "../libyuv/include/libyuv.h"
#include <pthread.h>


JXYUVEncodeH264::JXYUVEncodeH264(UserArguments *arg) : arguments(arg) {
    video_st = NULL;
    pFrame = NULL;
    pFormatCtx = NULL;
    swsContext = NULL;
    pFrameRGBA = NULL;
}

/**
 * 结束编码时刷出还在编码器里面的帧
 * @param fmt_ctx
 * @param stream_index
 * @return
 */
int JXYUVEncodeH264::flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
          CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
                                    NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        LOGI(JNI_DEBUG, "_Flush Encoder: Succeed to encode 1 frame video!\tsize:%5d\n",
             enc_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }

    return ret;
}

/**
 * 初始化视频编码器
 * @return
 */
int JXYUVEncodeH264::initVideoEncoder() {
    LOGI(JNI_DEBUG, "视频编码器初始化开始")

    size_t path_length = strlen(arguments->video_path);
    char *out_file = (char *) malloc(path_length + 1);
    strcpy(out_file, arguments->video_path);

    av_register_all();
    //Method1.
//    pFormatCtx = avformat_alloc_context();
//    //Guess Format
//    fmt = av_guess_format(NULL, out_file, NULL);
//
//    LOGE(JNI_DEBUG,",fmt==null?:%s", fmt == NULL ? "null" : "no_null");
//    pFormatCtx->oformat = fmt;
    //Method 2.
    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
    fmt = pFormatCtx->oformat;


    //Open output URL
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE(JNI_DEBUG, "_Failed to open output file! \n");
        return -1;
    }

    video_st = avformat_new_stream(pFormatCtx, 0);
    //video_st->time_base.num = 1;
    //video_st->time_base.den = 25;

    if (video_st == NULL) {
        LOGE(JNI_DEBUG, "_video_st==null");
        return -1;
    }

    //Param that must set
    pCodecCtx = video_st->codec;
    //pCodecCtx->codec_id =AV_CODEC_ID_HEVC;
    pCodecCtx->codec_id = AV_CODEC_ID_H264;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    pCodecCtx->width = arguments->in_width;
    pCodecCtx->height = arguments->in_height;


    pCodecCtx->bit_rate = arguments->video_bit_rate;
    pCodecCtx->gop_size = 50;
    pCodecCtx->thread_count = 12;

    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = arguments->frame_rate;
//    pCodecCtx->me_pre_cmp = 1;
    //H264
    //pCodecCtx->me_range = 16;
    //pCodecCtx->max_qdiff = 4;
    //pCodecCtx->qcompress = 0.6;
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51;

    //Optional Param
    pCodecCtx->max_b_frames = 3;

    // Set Option
    AVDictionary *param = 0;
    //H.264
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
//        av_dict_set(&param, "tune", "animation", 0);
//        av_dict_set(&param, "profile", "baseline", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
        av_opt_set(pCodecCtx->priv_data, "preset", "ultrafast", 0);
        av_dict_set(&param, "profile", "baseline", 0);
    }

    //Show some Information
    av_dump_format(pFormatCtx, 0, out_file, 1);

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
//    pCodec = avcodec_find_encoder_by_name("nvenc_h264");
    if (!pCodec) {
        LOGE(JNI_DEBUG, "Can not find encoder! \n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
        LOGE(JNI_DEBUG, "Failed to open encoder! \n");
        return -1;
    }


    pFrame = av_frame_alloc();
    picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    LOGI(JNI_DEBUG, "   picture_size:%d", picture_size);
    uint8_t *buf = (uint8_t *) av_malloc(picture_size);
    avpicture_fill((AVPicture *) pFrame, buf, pCodecCtx->pix_fmt, pCodecCtx->width,
                   pCodecCtx->height);

//    swsContext = sws_alloc_context();
//
//    swsContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGBA,
//          pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

    pFrameRGBA= av_frame_alloc();
    int len = avpicture_get_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height);

    uint8_t * out_buffer = (uint8_t *)av_malloc(len);

    avpicture_fill((AVPicture *)pFrameRGBA, out_buffer, AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height);

    //Write File Header
    avformat_write_header(pFormatCtx, NULL);
    av_new_packet(&pkt, picture_size);
    out_y_size = pCodecCtx->width * pCodecCtx->height;
    is_end = START_STATE;
    pthread_t thread;
    pthread_create(&thread, NULL, JXYUVEncodeH264::startEncode, this);

    LOGI(JNI_DEBUG, "视频编码器初始化完成")

    return 0;
}

/**
 * 发送一帧到编码队列
 * @param buf
 * @return
 */
int JXYUVEncodeH264::startSendOneFrame(void *buf) {
    int in_y_size = arguments->in_width * arguments->in_height;
    uint8_t *new_buf = (uint8_t *) malloc(in_y_size * 4);
    memcpy(new_buf, (uint8_t *)buf, in_y_size * 4);

    frame_queue.push(new_buf);

    return 0;
}

/**
 * 启动编码线程
 * @param obj
 * @return
 */
void *JXYUVEncodeH264::startEncode(void *obj) {
    JXYUVEncodeH264 *h264_encoder = (JXYUVEncodeH264 *) obj;
    if (h264_encoder == NULL) {
        return 0;
    }
    while (!h264_encoder->is_end||!h264_encoder->frame_queue.empty()) {
        if(h264_encoder->is_release){
            //Write file trailer
            delete h264_encoder;
            h264_encoder = NULL;
            return 0;
        }
        if (h264_encoder->frame_queue.empty()) {
            continue;
        }
        uint8_t *picture_buf = *h264_encoder->frame_queue.wait_and_pop().get();
        LOGI(JNI_DEBUG, "send_videoframe_count:%d", h264_encoder->frame_count);

        LOGI(JNI_DEBUG, "start RGBAToI420");

        int result = libyuv::ARGBToI420(picture_buf, h264_encoder->arguments->in_width*4,
                   h264_encoder->pFrame->data[0], h264_encoder->arguments->in_width,
                   h264_encoder->pFrame->data[2], h264_encoder->arguments->in_width/2,
                   h264_encoder->pFrame->data[1], h264_encoder->arguments->in_width/2,
                   h264_encoder->arguments->in_width,
                   h264_encoder->arguments->in_height);
        LOGI(JNI_DEBUG, "end RGBAToI420 result = %d", result);
        delete (picture_buf);

        //PTS
        h264_encoder->pFrame->pts = h264_encoder->frame_count;
        h264_encoder->frame_count++;
        int got_picture = 0;
        //Encode
        int ret = avcodec_encode_video2(h264_encoder->pCodecCtx, &h264_encoder->pkt,
                                        h264_encoder->pFrame, &got_picture);
        if (ret < 0) {
            LOGE(JNI_DEBUG, "Failed to encode! \n");
        }
        if (got_picture == 1) {
            LOGI(JNI_DEBUG, "Succeed to encode frame: %5d\tsize:%5d\n", h264_encoder->framecnt,
                 h264_encoder->pkt.size);
            h264_encoder->framecnt++;
            h264_encoder->pkt.stream_index = h264_encoder->video_st->index;
            ret = av_write_frame(h264_encoder->pFormatCtx, &h264_encoder->pkt);
            av_free_packet(&h264_encoder->pkt);
        }
    }
    if (h264_encoder->is_end) {
        h264_encoder->encodeEnd();
        delete h264_encoder;
        h264_encoder = NULL;
    }
    return 0;
}

/**
 * 视频编码结束
 * @return
 */
int JXYUVEncodeH264::encodeEnd() {
    //Flush Encoder
    int ret_1 = flush_encoder(pFormatCtx, 0);
    if (ret_1 < 0) {
        LOGE(JNI_DEBUG, "Flushing encoder failed\n");
        return -1;
    }
    LOGI(JNI_DEBUG, "视频编码结束")
    arguments->handler->setup_video_state(END_STATE);
//    arguments->handler->try_encode_over(arguments);

    return 1;
}

/**
 * 用户中断
 */
void JXYUVEncodeH264::user_end() {
    is_end = END_STATE;
}
void JXYUVEncodeH264::release()  {
    is_release = JX_TRUE;
}