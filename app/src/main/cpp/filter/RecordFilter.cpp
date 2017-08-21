//
// Created by Administrator on 2017/8/21 0021.
//


#include "BaseFilter.h"
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

    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[0]);
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPixelBuffers[1]);
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL, GL_STATIC_READ);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void RecordFilter::destroyPixelBuffer() {

}





















