/**
 * Created by jianxi on 2017/5/12.
 * https://github.com/mabeijianxi
 * mabeijianxi@gmail.com
 */
#ifndef JIANXIFFMPEG_JX_YUV_ENCODE_H264_H
#define JIANXIFFMPEG_JX_YUV_ENCODE_H264_H



#include "base_include.h"
#include "jx_user_arguments.h"

using namespace std;
/**
 * yuv编码h264
 */
typedef struct YUVData {
    uint8_t * yData;
    uint8_t * uData;
    uint8_t * vData;
};

class JXYUVEncodeH264 {
public:
    JXYUVEncodeH264(UserArguments* arg);
public:
    int initVideoEncoder();

    static void* startEncode(void * obj);

    int startSendOneFrame(void *buf);

    void user_end();

    void release();

    int encodeEnd();

    ~JXYUVEncodeH264() {
        //Clean
//        if (video_st != NULL) {
//            avcodec_close(video_st->codec);
//            av_free(pFrame);
//            video_st = NULL;
//            pFrame = NULL;
//        }
//        if (pFormatCtx != NULL) {
//            av_write_trailer(pFormatCtx);
//            avio_close(pFormatCtx->pb);
//            avformat_free_context(pFormatCtx);
//            pFormatCtx = NULL;
//        }
    }

private:
    int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

public:
    UserArguments *arguments;
private:
    int is_end = 0;
    int is_release = 0;
    threadsafe_queue<uint8_t *> frame_queue;
    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *video_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket pkt;
    AVFrame *pFrame;
    int picture_size;
    int out_y_size;
    int framecnt = 0;
    int frame_count = 0;
    int frame_count1 = 0;


};

#endif //JIANXIFFMPEG_JX_YUV_ENCODE_H264_H
