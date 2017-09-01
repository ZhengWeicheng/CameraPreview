//
// Created by Administrator on 2017/9/1 0001.
//

#include "BaseFilter.h"

#ifndef MYOPENGL2_RECORDCAMERAFILTER_H
#define MYOPENGL2_RECORDCAMERAFILTER_H
class RecordCameraFilter : public BaseFilter {
public:
    RecordCameraFilter() {
        mPboIndex = 0;
        mPboNewIndex = 0;
        mInitRecord = false;
        *mPixelBuffers = NULL;
        env = NULL;
    }

    ~RecordCameraFilter() {
        BaseFilter::~BaseFilter();
        if (mPixelBuffers != NULL) {
            destroyPixelBuffer();
        }
    }

    void onCreated();

    void onDrawArraysPre();

    void onDrawArraysAfter();

    void initFramebuffer(int width, int height);

    void destroyFrameBuffer();

    int drawToFrameBuffer(GLenum target, GLuint texture);

    void drawFrame(GLenum target, GLuint texture);

    void initPixelBuffer(int width, int height);

    void destroyPixelBuffer();

    void bindPixelBuffer();

    void unbindPixelBuffer();

    void onPrepareToRender();

    int mRowStride;
    int mPboIndex;
    int mPboNewIndex;
    int mPboSize;
    bool mInitRecord;
    JNIEnv *env; //env全局指针
    GLuint mPixelBuffers[2];
};
#endif //MYOPENGL2_RECORDCAMERAFILTER_H
