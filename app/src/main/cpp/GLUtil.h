
#ifndef MYOPENGLES_GL_UTILS_H
#define MYOPENGLES_GL_UTILS_H

#include <GLES2/gl2.h>
#include <jni.h>
#include "RenderHolder.h"

void checkGlError(const char *op);

GLuint loadShader(GLenum shaderType, const char *shaderSource);

GLuint createProgram(const char *pVertexSrc, const char *pFragmentSrc);

void createRenderHolder(JNIEnv *env, jobject jSurface, RenderHolder *holder);

GLuint* createExternalOESTexture();
// 角度转弧度
float d2r(float d);

#endif //MYOPENGLES_GL_UTILS_H
