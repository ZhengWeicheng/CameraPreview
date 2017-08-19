//
// Created by Administrator on 2017/8/19.
//

#include "BaseFilter.h"

#ifndef CAMERAPREVIEW_CAMERATEXTUREFILTER_H
#define CAMERAPREVIEW_CAMERATEXTUREFILTER_H
class CameraTextureFilter : BaseFilter {

public:
    CameraTextureFilter() {
        BaseFilter();
        posDistance = 0;
        posNextFilterId = 0;
        mDistance = 0.f;
        mNextFilterId = 0.f;
    }

    ~CameraTextureFilter() {
        BaseFilter::~BaseFilter();
    }

public:
    virtual void create(const char* vertex, const char* texture);

//    virtual void onDrawArraysAfter();

//    virtual void onCreated();

    virtual void drawFrame(GLenum target, GLuint texture, jfloat *mverMatrix, jfloat *mTmpMatrix);

    void setDistanceAndNextFilter(float distance, float nextFilter);

    bool isProgramAvailable();

private:
    virtual void onDrawArraysPre();

public:
    GLuint posDistance;

    GLuint posNextFilterId;

    float mDistance;

    float mNextFilterId;
};
#endif //CAMERAPREVIEW_CAMERATEXTUREFILTER_H
