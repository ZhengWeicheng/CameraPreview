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
        mverMatrix = NULL;
        mTmpMatrix = NULL;

    }

    ~CameraTextureFilter() {
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

private:
    virtual void onDrawArraysPre();

public:
    GLuint posDistance;

    GLuint posNextFilterId;

    GLuint posVertex;

    GLuint posTexMat;

    float mDistance;

    float mNextFilterId;

    jfloat *mverMatrix;

    jfloat *mTmpMatrix;
};
#endif //CAMERAPREVIEW_CAMERATEXTUREFILTER_H
