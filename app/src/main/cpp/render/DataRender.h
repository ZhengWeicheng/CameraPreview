

#ifndef MYOPENGLES_GL_NATIVE_LIB_H
#define MYOPENGLES_GL_NATIVE_LIB_H


#define  LOG_TAG    "render"

#include <jni.h>
#include <android/log.h>

#include "../GLContextHolder.h"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
JNIEXPORT jlong JNICALL Java_com_myopengl_zcweicheng_render_Renderer_nativeInit(JNIEnv *env,
                                                                                jclass type,
                                                                                jobject jSurface,
                                                                                jstring vertex,
                                                                                jstring fragment);

JNIEXPORT void JNICALL Java_com_myopengl_zcweicheng_render_Renderer_nativeRelease(JNIEnv *env,
                                                                                  jclass type,
                                                                                  jlong engine);

JNIEXPORT void JNICALL Java_com_myopengl_zcweicheng_render_Renderer_nativeRender(JNIEnv *env,
                                                                                 jclass,
                                                                                 jlong engine,
                                                                                 jbyteArray data_,
                                                                                 jint frameWidth,
                                                                                 jint frameHeight,
                                                                                 jint degree,
                                                                                 jboolean mirror,
                                                                                 jint outWidth,
                                                                                 jint outHeight,
                                                                                 jfloat distance,
                                                                                 jint nextFilterId,
                                                                                 jlong facePtr);

JNIEXPORT void JNICALL Java_com_myopengl_zcweicheng_render_Renderer_nativeSwitchFilter(JNIEnv *env, jobject instance,jlong engine,jobject mAssetManager, jint mFilterType,jstring vert,jstring frag);
}

void releaseGLContext(GLContextHolder *engineHolder);

void renderFrame(GLContextHolder *engineHolder, jbyte *data, jint width, jint height, jint degree,
                 jboolean mirror, jint outWidth, jint outHeight, jfloat distance, jint nextFilterId, jlong facePtr);

#endif //MYOPENGLES_GL_NATIVE_LIB_H