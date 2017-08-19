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
        ||holder->eglContext == EGL_NO_CONTEXT) {
        delete holder;
        return 0;
    }

    //context create success,now create program
    const char *vertex = env->GetStringUTFChars(vertex_, 0);
    const char *fragment = env->GetStringUTFChars(fragment_, 0);

    holder->mCameraFilter->create(vertex, fragment);

    env->ReleaseStringUTFChars(vertex_, vertex);
    env->ReleaseStringUTFChars(fragment_, fragment);
//    delete shader;
    if (!holder->mCameraFilter->isProgramAvailable()) {
        delete holder;
        return 0;
    }

    holder->updatImageMethodId = getSurfaceUpdateImageMethodId(env);

    // Use tightly packed data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //tex
    holder->textures = createExternalOESTexture();
    holder->textureNums = 1;

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

    holder->mCameraFilter->setDistanceAndNextFilter(distance, nextFilterId);

    holder->mCameraFilter->drawFrame(GL_TEXTURE_EXTERNAL_OES, holder->textures[0],
            mverMatrix, mTmpMatrix);

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