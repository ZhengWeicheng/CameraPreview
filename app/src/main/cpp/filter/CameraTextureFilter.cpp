//
// Created by Administrator on 2017/8/19.
//


#include "BaseFilter.h"
#include "CameraTextureFilter.h"

void CameraTextureFilter::create(const char *vertex, const char *texture) {
    BaseFilter::create(vertex, texture);

    posDistance = (GLuint) glGetUniformLocation(programId, "distance");

    posNextFilterId = (GLuint) glGetUniformLocation(programId, "currentFilter");

    posVertex = (GLuint) glGetUniformLocation(programId, "uMVPMatrix");

    posTexMat = (GLuint) glGetUniformLocation(programId, "uTexMatrix");
}

void CameraTextureFilter::onDrawArraysPre() {
    glUniformMatrix4fv(posVertex, 1, GL_FALSE, mverMatrix);
    glUniformMatrix4fv(posTexMat, 1, GL_FALSE, mTmpMatrix);

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
    return BaseFilter::isProgramAvailable();
}

void CameraTextureFilter::drawFrame(GLenum target, GLuint texture) {
    BaseFilter::drawFrame(target, texture);
}

void CameraTextureFilter::initFramebuffer(int width, int height) {
    BaseFilter::initFramebuffer(width, height);
}

int CameraTextureFilter::drawToFrameBuffer(GLenum target, GLuint texture, jfloat *mverMatrix,
                                           jfloat *mTmpMatrix) {
    this->mverMatrix = mverMatrix;
    this->mTmpMatrix = mTmpMatrix;
    return BaseFilter::drawToFrameBuffer(target, texture, mverMatrix, mTmpMatrix);
}

void CameraTextureFilter::setMatrix(jfloat *mverMatrix, jfloat *mTmpMatrix) {
    this->mverMatrix = mverMatrix;
    this->mTmpMatrix = mTmpMatrix;
}



















