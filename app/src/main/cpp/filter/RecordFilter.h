//
// Created by Administrator on 2017/8/21 0021.
//



#ifndef MYOPENGL2_RECORDFILTER_H
#define MYOPENGL2_RECORDFILTER_H
#include "BaseFilter.h"
#include "../encode/jx_yuv_encode_h264.h"
#include "../encode/jx_pcm_encode_aac.h"
//RGBA 4字节
#define PIXEL_STRIDE 4;

class RecordFilter : BaseFilter {
public:
    RecordFilter() {
        BaseFilter:BaseFilter();
        isEncode = false;
//        mRowStride = 0;
        mPboIndex = 0;
        mPboNewIndex = 0;
        mLastTimestamp = 0;//图像时间戳，用于录制帧数判断
        mRecordEnabled = false;
        mInitRecord = false;
        h264_encoder = NULL;
        *mPixelBuffers = NULL;
    }

    ~RecordFilter() {
        BaseFilter::~BaseFilter();
        if (mPixelBuffers != NULL) {
            destroyPixelBuffer();
        }
        if (h264_encoder != NULL) {
            delete(h264_encoder);
            h264_encoder = NULL;
        }
    }

     void create(const char* vertex, const char* texture);

     void onDrawArraysPre();

     void onDrawArraysAfter();

     void onCreated();

     void initFramebuffer(int width, int height);

     void destroyFrameBuffer();

     int drawToFrameBuffer(GLenum target, GLuint texture);

     void drawFrame(GLenum target, GLuint texture);

     bool isProgramAvailable();

    void initPixelBuffer(int width, int height);

    void destroyPixelBuffer();

    void bindPixelBuffer();

    void unbindPixelBuffer();

    void onPrepareToRender();

    void setFrameSize(int width, int height);

    void startRecord();

    void endRecord();

    void end_notify();
public:
    bool isEncode;
    JXYUVEncodeH264 *h264_encoder;
    int mRowStride;
    int mPboIndex;
    int mPboNewIndex;
    int mPboSize;
    long mLastTimestamp;//图像时间戳，用于录制帧数判断
    bool mRecordEnabled;
    bool mInitRecord;
//    GLuint* mPixelBuffers;
    GLuint mPixelBuffers[2];
};
#endif //MYOPENGL2_RECORDFILTER_H
