//
// Created by Administrator on 2017/8/19.
//

#ifndef CAMERAPREVIEW_CAMERATEXTUREFILTER_H
#define CAMERAPREVIEW_CAMERATEXTUREFILTER_H
#include "BaseFilter.h"
#include "../encode/jx_yuv_encode_h264.h"
#include "../encode/jx_pcm_encode_aac.h"

class CameraTextureFilter : BaseFilter {

public:
    CameraTextureFilter() {
        BaseFilter();
        posDistance = 0;
        posNextFilterId = 0;
        mDistance = 0.f;
        mNextFilterId = 0.f;
        mverMatrix = NULL;
        mTmpMatrix = NULL;
        isEncode = false;
        mRowStride = 0;
        mPboIndex = 0;
        mPboNewIndex = 0;
        mLastTimestamp = 0;//图像时间戳，用于录制帧数判断
        mRecordEnabled = false;
        mInitRecord = false;
        mPixelBuffers = NULL;
        h264_encoder = NULL;
        aac_encoder = NULL;

    }

    virtual  ~CameraTextureFilter() {
        if (mPixelBuffers != NULL) {
            destroyPixelBuffer();
        }
        if (h264_encoder != NULL) {
            delete(h264_encoder);
            h264_encoder = NULL;
        }
        if (aac_encoder != NULL) {
            delete(aac_encoder);
            aac_encoder = NULL;
        }
        BaseFilter::~BaseFilter();
    }

public:
    virtual void create(const char* vertex, const char* texture);

    virtual void initFramebuffer(int width, int height);

    virtual int drawToFrameBuffer(GLenum target, GLuint texture, jfloat *mverMatrix, jfloat *mTmpMatrix);

    virtual void drawFrame(GLenum target, GLuint texture);

    void setDistanceAndNextFilter(float distance, float nextFilter);

    void setMatrix(jfloat * mverMatrix, jfloat * mTmpMatrix);

    bool isProgramAvailable();

    void initPixelBuffer(int width, int height);

    void destroyPixelBuffer();

    void bindPixelBuffer();

    void unbindPixelBuffer();

private:
    void onDrawArraysPre();

    void onDrawArraysAfter();

public:
    GLuint posDistance;

    GLuint posNextFilterId;

    GLuint posVertex;

    GLuint posTexMat;

    float mDistance;

    float mNextFilterId;

    jfloat *mverMatrix;

    jfloat *mTmpMatrix;


    bool isEncode;

    JXYUVEncodeH264 *h264_encoder;
    JXPCMEncodeAAC *aac_encoder;

    int mRowStride;
    int mPboIndex;
    int mPboNewIndex;
    int mPboSize;
    long mLastTimestamp;//图像时间戳，用于录制帧数判断
    bool mRecordEnabled;
    bool mInitRecord;
    GLuint* mPixelBuffers;
};
#endif //CAMERAPREVIEW_CAMERATEXTUREFILTER_H
