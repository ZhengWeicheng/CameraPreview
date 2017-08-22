//
// Created by Administrator on 2017/8/21 0021.
//


#include "RecordFilter.h"

void RecordFilter::create(const char *vertex, const char *texture) {
    BaseFilter::create(vertex, texture);
}

void RecordFilter::onDrawArraysPre() {
    BaseFilter::onDrawArraysPre();
}

void RecordFilter::onDrawArraysAfter() {
    BaseFilter::onDrawArraysAfter();
}

void RecordFilter::onCreated() {
    BaseFilter::onCreated();
}

void RecordFilter::initFramebuffer(int width, int height) {
    BaseFilter::initFramebuffer(width, height);
}

void RecordFilter::destroyFrameBuffer() {
    BaseFilter::destroyFrameBuffer();
}

int RecordFilter::drawToFrameBuffer(GLenum target, GLuint texture) {
    return BaseFilter::drawToFrameBuffer(target, texture);
}

void RecordFilter::drawFrame(GLenum target, GLuint texture) {
    BaseFilter::drawFrame(target, texture);
}

bool RecordFilter::isProgramAvailable() {
    return BaseFilter::isProgramAvailable();
}

void RecordFilter::initPixelBuffer(int width, int height) {
    if (mPixelBuffers != NULL && (frameWidth != width || frameHeight != height)) {
        frameWidth = width;
        frameHeight = height;
        destroyPixelBuffer();
    }


    if (mPixelBuffers != NULL) {
        return;
    }

    int align = 128;//128字节对齐
    mRowStride = (width * 4 + (align - 1)) & ~(align - 1);

    mPboSize = mRowStride * height;

    mPixelBuffers = new GLuint[2];
    glGenBuffers(2, mPixelBuffers);
//
//    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[0]);
//    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);
//
//    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[1]);
//    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);
//
//    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void RecordFilter::destroyPixelBuffer() {
    if (mPixelBuffers != NULL) {
        glDeleteBuffers(2, mPixelBuffers);
        mPixelBuffers = NULL;
    }
}

void RecordFilter::bindPixelBuffer() {
//    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[mPboIndex]);
//    glReadPixels(0, 0, mRowStride / 4, frameHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//
//    if (mInitRecord) {//第一帧没有数据跳出
//        unbindPixelBuffer();
//        mInitRecord = false;
//        return;
//    }
//
//    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[mPboNewIndex]);
//
//    //glMapBufferRange会等待DMA传输完成，所以需要交替使用pbo
//    glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mPboSize, GL_MAP_READ_BIT);
//
//    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
//    unbindPixelBuffer();

}

//解绑pbo
void RecordFilter::unbindPixelBuffer() {
//    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
//
//    mPboIndex = (mPboIndex + 1) % 2;
//    mPboNewIndex = (mPboNewIndex + 1) % 2;
}