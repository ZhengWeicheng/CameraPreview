//
// Created by Administrator on 2017/8/18 0018.
//


#include <malloc.h>
#include "BaseFilter.h"
#include "../log.h"

#define LOG_TAG "BaseFilter"

void BaseFilter::create(const char *vertex, const char *texture) {
    programId = createProgram(vertex, texture);
    if (programId <= 0) {
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
    frameWidth = width;
    frameHeight = height;
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

int BaseFilter::drawToFrameBuffer(GLenum target, GLuint texture) {
    if (mFrameBuffers == NULL) {
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffers[0]);
//    glViewport(0, 0, frameWidth, frameHeight);
    drawFrame(target, texture);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return mFrameBufferTextures[0];
}

void BaseFilter::drawFrame(GLenum target, GLuint texture) {

    glUseProgram(programId);

//    onPrepareToRender();

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
    glDisableVertexAttribArray(posAttrVertices);
    glDisableVertexAttribArray(posAttrTexCoords);

    onDrawArraysAfter();

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

GLuint BaseFilter::loadShader(GLenum shaderType, const char *shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc((size_t) infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint BaseFilter::createProgram(const char *pVertexSrc, const char *pFragmentSrc) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSrc);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSrc);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glBindAttribLocation(program, 0, "vPosition");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc((size_t) bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

void BaseFilter::checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
}

void BaseFilter::onPrepareToRender() {
    glViewport(0, 0, frameWidth, frameHeight);
}

void BaseFilter::setFrameSize(int width, int height) {
    this->frameWidth = width;
    this->frameHeight = height;
}

















