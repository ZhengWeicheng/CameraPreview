//
// Created by Administrator on 2017/8/16 0016.
//

#include <jni.h>
#include "TextureRender.h"
#include "../utils/GLUtil.h"
#include "../utils/JNIUtils.h"
#include <GLES2/gl2ext.h>
#include <android/asset_manager_jni.h>
#include "../encode/jx_jni_handler.h"

JNIEXPORT jlong JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeInit(JNIEnv *env, jclass type,
        jobject surface, jobject assetManager,jint width,jint height) {
    TextureRenderHolder *holder = new TextureRenderHolder();

    createRenderHolder(env, surface, (RenderHolder*)holder);
    if (holder->eglDisplay == EGL_NO_DISPLAY || holder->eglSurface == EGL_NO_SURFACE
        ||holder->eglContext == EGL_NO_CONTEXT) {
        delete holder;
        return 0;
    }

    //context create success,now create program
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    const char* vertex = getGLSLStr(mgr, "vertex.glsl");
    const char* fragment = getGLSLStr(mgr, "slide_fragment.glsl");
    holder->mCameraFilter->create(vertex, fragment);

//    delete shader;
    if (!holder->mCameraFilter->isProgramAvailable()) {
        delete holder;
        return 0;
    }

    holder->mCameraFilter->initFramebuffer(width, height);

//    holder->mCameraFilter->initPixelBuffer(width, height);

//    const char* baseVertex = getGLSLStr(mgr, "ext_vertex.glsl");
//    const char* baseFragment = getGLSLStr(mgr, "ext_fragment.glsl");
//    holder->mFilter->create(baseVertex, baseFragment);
//    if (!holder->mFilter->isProgramAvailable()) {
//        delete holder;
//        return 0;
//    }
//    holder->mRecordFilter->create(baseVertex, baseFragment);
//    if (!holder->mRecordFilter->isProgramAvailable()) {
//        delete holder;
//        return 0;
//    }
//
//    holder->mRecordFilter->initFramebuffer(360, 640);
//
//    holder->mRecordFilter->initPixelBuffer(360, 640);
//
//    holder->mFilter->setFrameSize(width, height);
//    holder->mRecordFilter->setFrameSize(360, 640);
//    holder->mCameraFilter->setFrameSize(width, height);

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
                                                                      jlong holder_) {
    if (holder_ == 0) {
        return;
    }
    TextureRenderHolder *holder = (TextureRenderHolder*) holder_;
    eglMakeCurrent(holder->eglDisplay, holder->eglSurface, holder->eglSurface, holder->eglContext);
    if (holder->textures != NULL) {
        glDeleteTextures(holder->textureNums, holder->textures);
    }
    if (holder->mCameraFilter != NULL) {
        delete holder->mCameraFilter;
        holder->mCameraFilter = NULL;
    }
    if (holder->mFilter != NULL) {
        delete holder->mFilter;
        holder->mFilter = NULL;
    }
    if (holder->mRecordFilter != NULL) {
        delete holder->mRecordFilter;
        holder->mRecordFilter = NULL;
    }
    delete holder;

}


JNIEXPORT void JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeDraw(JNIEnv *env, jclass type,
        jobject surface, jfloatArray mverMatrix_, jfloatArray mTmpMatrix_, jfloat distance,
        jfloat nextFilterId, jlong holder_) {
    jfloat *mverMatrix = env->GetFloatArrayElements(mverMatrix_, NULL);
    jfloat *mTmpMatrix = env->GetFloatArrayElements(mTmpMatrix_, NULL);

    TextureRenderHolder *holder = (TextureRenderHolder*) holder_;

    eglMakeCurrent(holder->eglDisplay, holder->eglSurface, holder->eglSurface, holder->eglContext);

    env->CallVoidMethod(surface, holder->updatImageMethodId);

    holder->mCameraFilter->setDistanceAndNextFilter(distance, nextFilterId);

    holder->mCameraFilter->drawFrame(GL_TEXTURE_EXTERNAL_OES, holder->textures[0], mverMatrix, mTmpMatrix);

//    holder->mFilter->drawFrame(GL_TEXTURE_2D, (GLuint) textureId);
//
//    if (holder->mRecordFilter->isEncode) {
//        holder->mRecordFilter->drawToFrameBuffer(GL_TEXTURE_2D, (GLuint) textureId);
//    }

    eglSwapBuffers(holder->eglDisplay, holder->eglSurface);

    env->ReleaseFloatArrayElements(mverMatrix_, mverMatrix, 0);
    env->ReleaseFloatArrayElements(mTmpMatrix_, mTmpMatrix, 0);
}

JNIEXPORT jint JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeGetInputTex(JNIEnv *env, jclass type,
                                                                          jlong engine) {
    if (engine == 0) {
        return 0;
    }
    TextureRenderHolder *holder = (TextureRenderHolder*) engine;
    return holder->textures[0];
}

JNIEXPORT jlong JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeGetEglContext(JNIEnv *env,
                                                                            jclass type,
                                                                            jlong engine) {
    if (engine == 0) {
        return 0;
    }
    TextureRenderHolder *holder = (TextureRenderHolder*) engine;
    return (jlong) holder->eglContext;
}

JNIEXPORT jint JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_stopRecord(JNIEnv *env, jclass type, jlong engine) {

    if (engine == 0) {
        return 0;
    }
    TextureRenderHolder *holder = (TextureRenderHolder*) engine;
    holder->mRecordFilter->endRecord();

//    if (holder->mRecordFilter->aac_encoder != NULL) {
//        holder->mRecordFilter->aac_encoder->user_end();
//    }
    return 0;

}

JNIEXPORT jint JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_startRecord(JNIEnv *env, jclass type, jlong engine,
                                                                    jstring mediaBasePath_,
                                                                    jstring mediaName_, jint filter,
                                                                    jint in_width, jint in_height,
                                                                    jint out_width, jint out_height,
                                                                    jint frameRate,
                                                                    jlong bit_rate) {
    if (engine == 0) {
        return 0;
    }
    TextureRenderHolder *holder = (TextureRenderHolder*) engine;
    jclass class1 = env->FindClass("com/myopengl/zcweicheng/render/CameraTextureThread");
    jmethodID pID = env->GetStaticMethodID(class1, "onFrameAvailable", "(I)V");
    jclass global_class = (jclass) env->NewGlobalRef(type);
    holder->mRecordFilter->startRecord();
    holder->mRecordFilter->pID = pID;
    holder->mRecordFilter->env = env;
    holder->mRecordFilter->java_class = global_class;
    env->GetJavaVM(&(holder->mRecordFilter->javaVM));
    return 0;

}
