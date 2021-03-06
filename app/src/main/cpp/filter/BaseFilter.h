//
// Created by Administrator on 2017/8/18 0018.
//

#ifndef MYOPENGL2_BASEFILTER_H
#define MYOPENGL2_BASEFILTER_H
#include <jni.h>
#include <cwchar>
#include <GLES3/gl3.h>

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

    virtual void onPrepareToRender();

    virtual void onDrawArraysPre();

    virtual void onDrawArraysAfter();

    virtual void onCreated();

    virtual void initFramebuffer(int width, int height);

    virtual void destroyFrameBuffer();

    virtual int drawToFrameBuffer(GLenum target, GLuint texture);

    virtual void drawFrame(GLenum target, GLuint texture);

    virtual void setFrameSize(int width, int height);

    virtual bool isProgramAvailable();

    void checkGlError(const char *op);

private:
    GLuint createProgram(const char *pVertexSrc, const char *pFragmentSrc);
    GLuint loadShader(GLenum shaderType, const char *shaderSource);

protected:
    GLuint programId;
    GLuint posAttrVertices;
    GLuint posAttrTexCoords;

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
