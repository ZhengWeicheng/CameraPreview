//
// Created by Administrator on 2017/8/18 0018.
//

#ifndef MYOPENGL2_BASEFILTER_H
#define MYOPENGL2_BASEFILTER_H

#include <GLES2/gl2.h>
#include <jni.h>

class BaseFilter {
public:
    BaseFilter() {
        programId = 0;
        posAttrVertices = 0;
        posAttrTexCoords = 0;
        mFrameBuffers = NULL;
        mFrameBufferTextures = NULL;
        frameWidth = 0;
        frameHeight = 0;

    }

    virtual ~BaseFilter() {
        if (programId <= 0) {
            glDeleteProgram(programId);
            programId = 0;
        }
        destroyFrameBuffer();
    }

public:
    virtual void create(const char* vertex, const char* texture);

    virtual void onDrawArraysPre();

    virtual void onDrawArraysAfter();

    virtual void onCreated();

    void initFrambuffer(int width, int height);

    void destroyFrameBuffer();

    int drawToFrameBuffer(GLenum target, GLuint texture, jfloat *mverMatrix, jfloat *mTmpMatrix);

    virtual void drawFrame(GLenum target, GLuint texture, jfloat *mverMatrix, jfloat *mTmpMatrix);

protected:
    GLuint programId;
    GLuint posAttrVertices;
    GLuint posAttrTexCoords;
    GLuint posVertex;
    GLuint posTexMat;

    int frameWidth;
    int frameHeight;

    GLuint* mFrameBuffers;
    GLuint* mFrameBufferTextures;
};

const GLfloat VERTICES_RENDER[] =
        {
                -1.0f, 1.0f,//pos0
                -1.0f, -1.0f,//pos1
                1.0f, -1.0f,//pos2
                1.0f, 1.0f,//pos3

        };

const GLfloat TEXTURE_RENDER[] =
        {
                1.0f, 0.0f,//tex0
                1.0f, 1.0f,//tex1
                0.0f, 1.0f,//tex2
                0.0f, 0.0f,//tex3

        };
#endif //MYOPENGL2_BASEFILTER_H
