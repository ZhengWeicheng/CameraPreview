//
// Created by Administrator on 2017/8/19.
//


#include "BaseFilter.h"
#include "CameraTextureFilter.h"

void CameraTextureFilter::create(const char *vertex, const char *texture) {
    BaseFilter::create(vertex, texture);

    posDistance = (GLuint) glGetUniformLocation(programId, "distance");

    posNextFilterId = (GLuint) glGetUniformLocation(programId, "currentFilter");
}

void CameraTextureFilter::onDrawArraysPre() {
    glUniform1f(posDistance, mDistance);
    glUniform1f(posNextFilterId, mNextFilterId);
}


//void CameraTextureFilter::onCreated() {
//    BaseFilter::onCreated();
//}

void CameraTextureFilter::setDistanceAndNextFilter(float distance, float nextFilter) {
    this->mDistance = distance;
    this->mNextFilterId = nextFilter;
}

bool CameraTextureFilter::isProgramAvailable() {
    return programId > 0;
}

void CameraTextureFilter::drawFrame(GLenum target, GLuint texture, jfloat *mverMatrix,
                                    jfloat *mTmpMatrix) {
    BaseFilter::drawFrame(target, texture, mverMatrix, mTmpMatrix);
}













