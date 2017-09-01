//
// Created by Administrator on 2017/9/1 0001.
//

#include <jni.h>
#include <android/asset_manager_jni.h>
#include "RecordCameraRender.h"
#include "../utils/GLUtil.h"
#include "../log.h"
#include "DataRender.h"
#include "../utils/JNIUtils.h"
#include <GLES2/gl2ext.h>

extern "C" {
    JNIEXPORT jlong JNICALL
    Java_com_myopengl_zcweicheng_render_RecordCameraRender_nativeInit(JNIEnv *env, jclass type,
            jint width, jint height, jint shareTextureId, jlong shareContext, jobject assetManager) {
        RecordCameraRender *holder = new RecordCameraRender();
        holder->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (holder->eglDisplay == EGL_NO_DISPLAY) {
            checkGlError("eglGetDisplay");
            delete (&holder);
            return 0;
        }
        EGLint majorVer, minVer;
        if (!eglInitialize(holder->eglDisplay, &majorVer, &minVer)) {
            checkGlError("eglInitialize");
            LOGE("eglInitialize");
            delete (&holder);
            return 0;
        }
        // EGL attributes
        const EGLint confAttr[] =
                {
                        EGL_RENDERABLE_TYPE, 0x0040,// very important!
                        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
                        EGL_RED_SIZE, 8,
                        EGL_GREEN_SIZE, 8,
                        EGL_BLUE_SIZE, 8,
                        EGL_ALPHA_SIZE, 8,
                        EGL_DEPTH_SIZE, 0,
                        EGL_STENCIL_SIZE, 0,
                        EGL_NONE
                };

        EGLConfig config;
        EGLint numConfigs;
        if (!eglChooseConfig(holder->eglDisplay, confAttr, &config, 1, &numConfigs)) {
            checkGlError("eglChooseConfig");
            delete (&holder);
            return 0;
        }
        EGLint attrib_list[] =
                {
                        EGL_CONTEXT_CLIENT_VERSION, 3,
                        EGL_NONE
                };
        EGLContext context = (EGLContext) shareContext;
        holder->eglContext = eglCreateContext(holder->eglDisplay, config, context, attrib_list);
        if (holder->eglContext == EGL_NO_CONTEXT) {
            checkGlError("eglCreateContext");
            delete (&holder);
            return 0;
        }

        EGLint surfaceAttribs[] = {
                EGL_WIDTH, width,
                EGL_HEIGHT, height,
                EGL_NONE
        };
        holder->eglSurface = eglCreatePbufferSurface(holder->eglDisplay, config, surfaceAttribs);
        checkGlError("eglCreatePbufferSurface");
        if (holder->eglSurface == EGL_NO_SURFACE) {
            delete (&holder);
            return 0;
        }


        if (!eglMakeCurrent(holder->eglDisplay,  holder->eglSurface,  holder->eglSurface,  holder->eglContext)) {
            checkGlError("eglMakeCurrent");
            delete (&holder);
            return 0;
        }

        holder->mShareTextureId = (GLuint) shareTextureId;

        //context create success,now create program
        AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
        const char* baseVertex = getGLSLStr(mgr, "ext_vertex.glsl");
        const char* baseFragment = getGLSLStr(mgr, "ext_fragment.glsl");
        holder->mRecordFilter->create(baseVertex, baseFragment);
        if (!holder->mRecordFilter->isProgramAvailable()) {
            delete holder;
            return 0;
        }
        holder->mRecordFilter->initFramebuffer(width, height);

        holder->mRecordFilter->initPixelBuffer(width, height);

        holder->mRecordFilter->setFrameSize(width, height);

        return (jlong) holder;
    }

    JNIEXPORT void JNICALL
    Java_com_myopengl_zcweicheng_render_RecordCameraRender_nativeRelease(JNIEnv *env, jclass type,
            jlong engine) {
        if (engine == 0) {
            return;
        }

        RecordCameraRender *holder = (RecordCameraRender*) engine;
        eglMakeCurrent(holder->eglDisplay, holder->eglSurface, holder->eglSurface, holder->eglContext);
        glDeleteTextures(1, &holder->mShareTextureId);
        if (holder->mRecordFilter != NULL) {
            delete holder->mRecordFilter;
            holder->mRecordFilter = NULL;
        }
        delete holder;
    }

    JNIEXPORT void JNICALL
    Java_com_myopengl_zcweicheng_render_RecordCameraRender_nativeProcess(JNIEnv *env, jclass type,
            jlong engine) {

        if (engine == 0) {
            return;
        }

        RecordCameraRender *holder = (RecordCameraRender*) engine;

        eglMakeCurrent(holder->eglDisplay, holder->eglSurface, holder->eglSurface, holder->eglContext);

        holder->mRecordFilter->drawToFrameBuffer(GL_TEXTURE_EXTERNAL_OES, (GLuint) holder->mShareTextureId);

        eglSwapBuffers(holder->eglDisplay, holder->eglSurface);
    }
}