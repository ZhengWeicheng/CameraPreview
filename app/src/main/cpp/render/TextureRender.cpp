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

    holder->mCameraFilter->initPixelBuffer(width, height);

    const char* baseVertex = getGLSLStr(mgr, "ext_vertex.glsl");
    const char* baseFragment = getGLSLStr(mgr, "ext_fragment.glsl");
    holder->mFilter->create(baseVertex, baseFragment);
    if (!holder->mFilter->isProgramAvailable()) {
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

    int textureId = holder->mCameraFilter->drawToFrameBuffer(GL_TEXTURE_EXTERNAL_OES,
            holder->textures[0], mverMatrix, mTmpMatrix);

    holder->mFilter->drawFrame(GL_TEXTURE_2D, (GLuint) textureId);

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
    holder->mCameraFilter->isEncode = false;
    if (holder->mCameraFilter->h264_encoder != NULL) {
        holder->mCameraFilter->h264_encoder->user_end();
    }

    if (holder->mCameraFilter->aac_encoder != NULL) {
        holder->mCameraFilter->aac_encoder->user_end();
    }
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

    jclass global_class = (jclass) env->NewGlobalRef(type);
    UserArguments *arguments = (UserArguments *) malloc(sizeof(UserArguments));
    const char *media_base_path = env->GetStringUTFChars(mediaBasePath_, 0);
    const char *media_name = env->GetStringUTFChars(mediaName_, 0);
    JXJNIHandler *jni_handler = new JXJNIHandler();
    jni_handler->setup_audio_state(START_STATE);
    jni_handler->setup_video_state(START_STATE);
    arguments->media_base_path = media_base_path;
    arguments->media_name = media_name;

    size_t v_path_size = strlen(media_base_path) + strlen(media_name) + strlen(VIDEO_FORMAT) + 1;
    arguments->video_path = (char *) malloc(v_path_size + 1);

    size_t a_path_size = strlen(media_base_path) + strlen(media_name) + strlen(AUDIO_FORMAT) + 1;
    arguments->audio_path = (char *) malloc(a_path_size + 1);

    size_t m_path_size = strlen(media_base_path) + strlen(media_name) + strlen(MEDIA_FORMAT) + 1;
    arguments->media_path = (char *) malloc(m_path_size + 1);

    strcpy(arguments->video_path, media_base_path);
    strcat(arguments->video_path, "/");
    strcat(arguments->video_path, media_name);
    strcat(arguments->video_path, VIDEO_FORMAT);

    strcpy(arguments->audio_path, media_base_path);
    strcat(arguments->audio_path, "/");
    strcat(arguments->audio_path, media_name);
    strcat(arguments->audio_path, AUDIO_FORMAT);

    strcpy(arguments->media_path, media_base_path);
    strcat(arguments->media_path, "/");
    strcat(arguments->media_path, media_name);
    strcat(arguments->media_path, MEDIA_FORMAT);

    arguments->video_bit_rate = bit_rate;
    arguments->frame_rate = frameRate;
    arguments->audio_bit_rate = 40000;
    arguments->audio_sample_rate = 44100;
    arguments->in_width = in_width;
    arguments->in_height = in_height;
    arguments->out_height = out_height;
    arguments->out_width = out_width;
    arguments->handler = jni_handler;
    arguments->env = env;
    arguments->java_class = global_class;
    arguments->env->GetJavaVM(&arguments->javaVM);
    holder->mCameraFilter->h264_encoder = new JXYUVEncodeH264(arguments);
//    holder->aac_encoder = new JXPCMEncodeAAC(arguments);
    int v_code = holder->mCameraFilter->h264_encoder->initVideoEncoder();
//    int a_code = holder->aac_encoder->initAudioEncoder();
    if (v_code == 0) {
        holder->mCameraFilter->isEncode = true;
        return 0;
    } else {
        return -1;
    }
}
