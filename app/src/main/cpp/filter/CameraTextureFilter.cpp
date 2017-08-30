//
// Created by Administrator on 2017/8/19.
//


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


void CameraTextureFilter::onDrawArraysAfter() {
    if (isEncode) {
        bindPixelBuffer();
    }
}

int CameraTextureFilter::drawToFrameBuffer(GLenum target, GLuint texture, jfloat *mverMatrix,
                                           jfloat *mTmpMatrix) {
    this->mverMatrix = mverMatrix;
    this->mTmpMatrix = mTmpMatrix;
    return BaseFilter::drawToFrameBuffer(target, texture);
}

void CameraTextureFilter::setMatrix(jfloat *mverMatrix, jfloat *mTmpMatrix) {
    this->mverMatrix = mverMatrix;
    this->mTmpMatrix = mTmpMatrix;
}

void CameraTextureFilter::initPixelBuffer(int width, int height) {
    if (mPixelBuffers != NULL && (frameWidth != width || frameHeight != height)) {
        destroyPixelBuffer();
    }
    frameWidth = width;
    frameHeight = height;

    if (mPixelBuffers != NULL) {
        return;
    }

    int align = 128;//128字节对齐
//    mRowStride = (width * 4 + (align - 1)) & ~(align - 1);

    mPboSize = width * height;

    mPixelBuffers = new GLuint[2];
    glGenBuffers(2, mPixelBuffers);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void CameraTextureFilter::destroyPixelBuffer() {
    if (mPixelBuffers != NULL) {
        glDeleteBuffers(2, mPixelBuffers);
        mPixelBuffers = NULL;
    }
}

void CameraTextureFilter::bindPixelBuffer() {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[mPboIndex]);
    glReadPixels(0, 0, frameWidth, frameHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    checkGlError("glReadPixels");
    if (mInitRecord) {//第一帧没有数据跳出
        unbindPixelBuffer();
        mInitRecord = false;
        return;
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[mPboNewIndex]);

    //glMapBufferRange会等待DMA传输完成，所以需要交替使用pbo
    void* buffer = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mPboSize, GL_MAP_READ_BIT);
    if (h264_encoder != NULL && buffer != NULL) {
        h264_encoder->startSendOneFrame(buffer);
    }
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    unbindPixelBuffer();

}

//解绑pbo
void CameraTextureFilter::unbindPixelBuffer() {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    mPboIndex = (mPboIndex + 1) % 2;
    mPboNewIndex = (mPboNewIndex + 1) % 2;
}

void CameraTextureFilter::onPrepareToRender() {
    glViewport(0, 0, frameWidth, frameHeight);
}

void CameraTextureFilter::setFrameSize(int width, int height) {
    BaseFilter::setFrameSize(width, height);
}




























