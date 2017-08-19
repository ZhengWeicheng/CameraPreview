//
// Created by Administrator on 2017/8/16 0016.
//

#ifndef MYOPENGL2_TEXTURERENDER_H
#define MYOPENGL2_TEXTURERENDER_H

#include <jni.h>
#include "../RenderHolder.h"
#include "../filter/BaseFilter.h"
#include "../filter/CameraTextureFilter.h"

class TextureRenderHolder : public RenderHolder {
public:
    TextureRenderHolder() {
        RenderHolder();
        mCameraFilter = new CameraTextureFilter();
        textures = NULL;
    }

    virtual ~TextureRenderHolder() {
        RenderHolder::~RenderHolder();
        if (textures != NULL) {
            glDeleteTextures(textureNums, textures);
        }
        if (mCameraFilter != NULL) {
            delete mCameraFilter;
            mCameraFilter = NULL;
        }
    }
public:
    int textureNums;

    GLuint *textures;

    jmethodID updatImageMethodId;

    CameraTextureFilter* mCameraFilter;
};


extern "C" {
JNIEXPORT jlong JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeInit(JNIEnv *env, jclass type,
                                                                   jobject surface, jstring vertex_,
                                                                   jstring fragment_);
JNIEXPORT void JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeRelease(JNIEnv *env, jclass type,
                                                                              jlong holder);

JNIEXPORT void JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeDraw(JNIEnv *env, jclass type,
                                                                           jobject surface,
                                                                           jfloatArray mverMatrix_,
                                                                           jfloatArray mTmpMatrix_,
                                                                           jfloat distance,
                                                                           jfloat nextFilterId,
                                                                           jlong holder);
JNIEXPORT jint JNICALL
        Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeGetInputTex(JNIEnv *env, jclass type,
                                                                                  jlong engine);

JNIEXPORT jlong JNICALL
        Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeGetEglContext(JNIEnv *env,
                                                                                    jclass type,
                                                                                    jlong engine);
}
#endif //MYOPENGL2_TEXTURERENDER_H
