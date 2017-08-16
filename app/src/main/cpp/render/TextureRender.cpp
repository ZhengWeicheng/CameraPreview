//
// Created by Administrator on 2017/8/16 0016.
//

#include <jni.h>
#include "TextureRender.h"
#include "../GLUtil.h"
#include "../JNIUtils.h"
#include <GLES2/gl2ext.h>

JNIEXPORT jlong JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeInit(JNIEnv *env, jclass type,
                                                                   jobject surface, jstring vertex_,
                                                                   jstring fragment_) {
    TextureRenderHolder *holder = new TextureRenderHolder();

    createRenderHolder(env, surface, vertex_, fragment_, (RenderHolder*)holder);
    if (holder->eglDisplay == EGL_NO_DISPLAY || holder->eglSurface == EGL_NO_SURFACE
        ||holder->eglContext == EGL_NO_CONTEXT || holder->currentProgram == 0) {
        delete holder;
        return 0;
    }
    holder->posAttrVertices = (GLuint) glGetAttribLocation(holder->currentProgram, "aPosition");

    holder->posAttrTexCoords = (GLuint) glGetAttribLocation(holder->currentProgram, "aTextureCoord");

    holder->posVertex = (GLuint) glGetUniformLocation(holder->currentProgram, "uMVPMatrix");

    holder->posTexMat = (GLuint) glGetUniformLocation(holder->currentProgram, "uTexMatrix");

    holder->posDistance = (GLuint) glGetUniformLocation(holder->currentProgram, "distance");

    holder->posNextFilterId = (GLuint) glGetUniformLocation(holder->currentProgram, "currentFilter");

    holder->updatImageMethodId = getSurfaceUpdateImageMethodId(env);

    glUseProgram(holder->currentProgram);

    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //tex
    GLuint *textures = new GLuint[1];
    glGenTextures(1, textures);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    holder->textureNums = 1;
    holder->textures = textures;


    glDepthMask(GL_FALSE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DITHER);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    return (jlong) holder;
}


JNIEXPORT void JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeRelease(JNIEnv *env, jclass type,
                                                                      jlong holder) {
    if (holder == 0) {
        return;
    }
    delete (TextureRenderHolder*) holder;

}


JNIEXPORT void JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeDraw(JNIEnv *env, jclass type,
                                                                   jobject surface,
                                                                   jfloatArray mverMatrix_,
                                                                   jfloatArray mTmpMatrix_,
                                                                   jfloat distance,
                                                                   jfloat nextFilterId,
                                                                   jlong holder_) {
    jfloat *mverMatrix = env->GetFloatArrayElements(mverMatrix_, NULL);
    jfloat *mTmpMatrix = env->GetFloatArrayElements(mTmpMatrix_, NULL);

    TextureRenderHolder *holder = (TextureRenderHolder*) holder_;

    eglMakeCurrent(holder->eglDisplay, holder->eglSurface, holder->eglSurface, holder->eglContext);

    env->CallVoidMethod(surface, holder->updatImageMethodId);

    //输入顶点
    glEnableVertexAttribArray(holder->posAttrVertices);
    glVertexAttribPointer(holder->posAttrVertices, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), VERTICES_RENDER);

    //输入纹理坐标
    glEnableVertexAttribArray(holder->posAttrTexCoords);
    glVertexAttribPointer(holder->posAttrTexCoords, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), TEXTURE_RENDER);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, holder->textures[0]);

    glUniformMatrix4fv(holder->posVertex, 1, GL_FALSE, mverMatrix);

    glUniformMatrix4fv(holder->posTexMat, 1, GL_FALSE, mTmpMatrix);

    glUniform1f(holder->posDistance, distance);
    glUniform1f(holder->posNextFilterId, nextFilterId);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(holder->posAttrVertices);
    glDisableVertexAttribArray(holder->posAttrTexCoords);

    eglSwapBuffers(holder->eglDisplay, holder->eglSurface);

    env->ReleaseFloatArrayElements(mverMatrix_, mverMatrix, 0);
    env->ReleaseFloatArrayElements(mTmpMatrix_, mTmpMatrix, 0);
}

JNIEXPORT jint JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeGetInputTex(JNIEnv *env, jclass type,
                                                                          jlong engine) {
    TextureRenderHolder *holder = (TextureRenderHolder*) engine;
    return holder->textures[0];
}

JNIEXPORT jlong JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeGetEglContext(JNIEnv *env,
                                                                            jclass type,
                                                                            jlong engine) {
    TextureRenderHolder *holder = (TextureRenderHolder*) engine;
    return (jlong) holder->eglContext;
}