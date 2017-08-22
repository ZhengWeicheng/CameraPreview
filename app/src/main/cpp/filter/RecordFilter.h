//
// Created by Administrator on 2017/8/21 0021.
//



#ifndef MYOPENGL2_RECORDFILTER_H
#define MYOPENGL2_RECORDFILTER_H
#include "BaseFilter.h"
//RGBA 4字节
#define PIXEL_STRIDE 4;

class RecordFilter : BaseFilter {
public:
    RecordFilter() {
        BaseFilter:BaseFilter();
        mRowStride = 0;
        mPboIndex = 0;
        mPboNewIndex = 0;
        mLastTimestamp = 0;//图像时间戳，用于录制帧数判断
        mRecordEnabled = false;
        mInitRecord = false;
        mPixelBuffers = NULL;
    }

    ~RecordFilter() {
        if (mPixelBuffers != NULL) {
            destroyPixelBuffer();
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


private:
    int mRowStride;
    int mPboIndex;
    int mPboNewIndex;
    int mPboSize;
    long mLastTimestamp;//图像时间戳，用于录制帧数判断
    bool mRecordEnabled;
    bool mInitRecord;
    GLuint* mPixelBuffers;

};
#endif //MYOPENGL2_RECORDFILTER_H
