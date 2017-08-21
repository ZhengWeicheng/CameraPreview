//
// Created by Administrator on 2017/8/18 0018.
//


#include "BaseFilter.h"
#include "../GLUtil.h"

void BaseFilter::create(const char *vertex, const char *texture) {
    programId = createProgram(vertex, texture);
    if (programId < 0) {
        delete this;
    }

    posAttrVertices = (GLuint) glGetAttribLocation(programId, "aPosition");

    posAttrTexCoords = (GLuint) glGetAttribLocation(programId, "aTextureCoord");

    onCreated();
}

void BaseFilter::initFramebuffer(int width, int height) {
    if (mFrameBuffers != NULL && (frameWidth != width || frameHeight != height)) {
        destroyFrameBuffer();
    }
    if (mFrameBuffers != NULL) {
        return;
    }
    mFrameBuffers = new GLuint[1];
    mFrameBufferTextures = new GLuint[1];
    glGenFramebuffers(1, mFrameBuffers);
    glGenTextures(1, mFrameBufferTextures);
    glBindTexture(GL_TEXTURE_2D, mFrameBufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffers[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFrameBufferTextures[0], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BaseFilter::destroyFrameBuffer() {
    if (mFrameBufferTextures != NULL) {
        glDeleteTextures(1, mFrameBufferTextures);
        mFrameBufferTextures = NULL;
    }
    if (mFrameBuffers != NULL) {
        glDeleteFramebuffers(1, mFrameBuffers);
        mFrameBuffers = NULL;
    }

}

int BaseFilter::drawToFrameBuffer(GLenum target, GLuint texture, jfloat *mverMatrix,
                                  jfloat *mTmpMatrix) {
    if (mFrameBuffers == NULL) {
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffers[0]);
//    glViewport(0, 0, frameWidth, frameHeight);
    drawFrame(target, texture, mverMatrix, mTmpMatrix);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return mFrameBufferTextures[0];
}

void BaseFilter::drawFrame(GLenum target, GLuint texture) {

    glUseProgram(programId);
    //输入顶点
    glEnableVertexAttribArray(posAttrVertices);
    glVertexAttribPointer(posAttrVertices, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), VERTICES_RENDER);

    //输入纹理坐标
    glEnableVertexAttribArray(posAttrTexCoords);
    glVertexAttribPointer(posAttrTexCoords, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), TEXTURE_RENDER);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(target, texture);

    onDrawArraysPre();

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    onDrawArraysAfter();

    glDisableVertexAttribArray(posAttrVertices);
    glDisableVertexAttribArray(posAttrTexCoords);
    glBindTexture(target, 0);
}


void BaseFilter::onDrawArraysPre() {

}

void BaseFilter::onDrawArraysAfter() {

}

void BaseFilter::onCreated() {

}

bool BaseFilter::isProgramAvailable() {
    return programId > 0;
}












