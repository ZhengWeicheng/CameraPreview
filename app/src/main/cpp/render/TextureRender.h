//
// Created by Administrator on 2017/8/16 0016.
//

#ifndef MYOPENGL2_TEXTURERENDER_H
#define MYOPENGL2_TEXTURERENDER_H

#define VIDEO_FORMAT ".h264"
#define MEDIA_FORMAT ".mp4"
#define AUDIO_FORMAT ".aac"

#include <jni.h>
#include "../RenderHolder.h"
#include "../filter/BaseFilter.h"
#include "../filter/CameraTextureFilter.h"
#include "../encode/jx_yuv_encode_h264.h"
#include "../encode/jx_pcm_encode_aac.h"
#include "../filter/RecordFilter.h"

class TextureRenderHolder : public RenderHolder {
public:
    TextureRenderHolder() {
        RenderHolder();
        mCameraFilter = new CameraTextureFilter();
        mFilter = new BaseFilter();
        mRecordFilter = new RecordFilter();
        textures = NULL;

    }

    virtual ~TextureRenderHolder() {
        RenderHolder::~RenderHolder();
    }
public:
    int textureNums;

    GLuint *textures;

    jmethodID updatImageMethodId;

    CameraTextureFilter* mCameraFilter;

    BaseFilter* mFilter;

    RecordFilter* mRecordFilter;
};


extern "C" {
JNIEXPORT jlong JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_nativeInit(JNIEnv *env, jclass type,
        jobject surface, jobject assetManager,jint width,jint height);
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

JNIEXPORT jint JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_stopRecord(JNIEnv *env, jclass type, jlong engine);

JNIEXPORT jint JNICALL
Java_com_myopengl_zcweicheng_render_CameraTextureThread_startRecord(JNIEnv *env, jclass type, jlong engine,
                                                                    jstring mediaBasePath_,
                                                                    jstring mediaName_, jint filter,
                                                                    jint in_width, jint in_height,
                                                                    jint out_width, jint out_height,
                                                                    jint frameRate,
                                                                    jlong bit_rate);

static void onDataCallback(JXYUVEncodeH264 *h264_encoder, uint8_t *buf);
}
#endif //MYOPENGL2_TEXTURERENDER_H
