//
// Created by Administrator on 2017/9/1 0001.
//


#include <malloc.h>
#include "RecordCameraFilter.h"
#include "../encode/jx_ffmpeg_config.h"
#include "../encode/jx_log.h"

void RecordCameraFilter::onCreated() {
    BaseFilter::onCreated();
}

void RecordCameraFilter::onDrawArraysPre() {
}

void RecordCameraFilter::onDrawArraysAfter() {
    bindPixelBuffer();
}

void RecordCameraFilter::initFramebuffer(int width, int height) {
    BaseFilter::initFramebuffer(width, height);
}

void RecordCameraFilter::destroyFrameBuffer() {
    BaseFilter::destroyFrameBuffer();
}

int RecordCameraFilter::drawToFrameBuffer(GLenum target, GLuint texture) {
    return BaseFilter::drawToFrameBuffer(target, texture, VERTICES_RENDER, TEXTURE_270_RENDER);
}

void RecordCameraFilter::drawFrame(GLenum target, GLuint texture) {
    BaseFilter::drawFrame(target, texture);
}

void RecordCameraFilter::initPixelBuffer(int width, int height) {
    if (*mPixelBuffers != NULL && (frameWidth != width || frameHeight != height)) {
        destroyPixelBuffer();
    }
    if (*mPixelBuffers != NULL) {
        return;
    }

    int align = 4;//128字节对齐
    //这个字节对齐很重要，如果没有字节对齐，效率上不会有任何的提升
//    mRowStride = (width*4 + (align - 1)) & ~(align - 1);
    mRowStride = (width*4 + (align - 1)) & ~(align - 1);
    mPboSize = mRowStride * height;

    glGenBuffers(2, mPixelBuffers);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void RecordCameraFilter::destroyPixelBuffer() {
    if (*mPixelBuffers != NULL) {
        glDeleteBuffers(2, mPixelBuffers);
        *mPixelBuffers = NULL;
    }
}

void RecordCameraFilter::bindPixelBuffer() {
    if (*mPixelBuffers == NULL) {
        return;
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[mPboIndex]);
    glReadPixels(0, 0, mRowStride / 4, frameHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    if (mInitRecord) {//第一帧没有数据跳出
        unbindPixelBuffer();
        mInitRecord = false;
        return;
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[mPboNewIndex]);

    //glMapBufferRange会等待DMA传输完成，所以需要交替使用pbo
    uint8_t * buffer = (uint8_t *) glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, mPboSize, GL_MAP_READ_BIT);
    encodeFrame2H264(buffer);

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    unbindPixelBuffer();
}

void RecordCameraFilter::unbindPixelBuffer() {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    mPboIndex = (mPboIndex + 1) % 2;
    mPboNewIndex = (mPboNewIndex + 1) % 2;
}

void RecordCameraFilter::onPrepareToRender() {
    BaseFilter::onPrepareToRender();
}























