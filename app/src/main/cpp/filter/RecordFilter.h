//
// Created by Administrator on 2017/8/21 0021.
//



#ifndef MYOPENGL2_RECORDFILTER_H
#define MYOPENGL2_RECORDFILTER_H
#include "BaseFilter.h"
#include "../encode/jx_yuv_encode_h264.h"
#include "../encode/jx_pcm_encode_aac.h"
#include <jni.h>
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
        mInitRecord = false;
        *mPixelBuffers = NULL;
        env = NULL;
        javaVM = NULL;
        java_class = NULL;
    }

    ~RecordFilter() {
        BaseFilter::~BaseFilter();
        if (mPixelBuffers != NULL) {
            destroyPixelBuffer();
        }
        if (java_class != NULL) {
            env->DeleteGlobalRef(java_class);
            env = NULL;
            javaVM = NULL;
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
    int mRowStride;
    int mPboIndex;
    int mPboNewIndex;
    int mPboSize;
    bool mInitRecord;
    jmethodID pID;
    JNIEnv *env; //env全局指针
    JavaVM *javaVM; //jvm指针
    jclass java_class; //java接口类的calss对象
    GLuint mPixelBuffers[2];
};
#endif //MYOPENGL2_RECORDFILTER_H
