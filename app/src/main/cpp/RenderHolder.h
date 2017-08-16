//
// Created by Administrator on 2017/8/16 0016.
//

#ifndef MYOPENGL2_RENDERHOLDER_H
#define MYOPENGL2_RENDERHOLDER_H

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/log.h>

class RenderHolder {
public:
    RenderHolder() {
        eglDisplay = EGL_NO_DISPLAY;
        eglSurface = EGL_NO_SURFACE;
        eglContext = EGL_NO_CONTEXT;
        currentProgram = 0;
    }

    virtual ~RenderHolder() {
        if (currentProgram <= 0) {
            glDeleteProgram(currentProgram);
            currentProgram = 0;
        }
        if (eglDisplay != EGL_NO_DISPLAY) {
            eglDestroySurface(eglDisplay, eglSurface);
            eglDestroyContext(eglDisplay, eglContext);
//            eglReleaseThread();
            eglTerminate(eglDisplay);
        }

        eglDisplay = EGL_NO_DISPLAY;
        eglSurface = EGL_NO_SURFACE;
        eglContext = EGL_NO_CONTEXT;
    }

public:
    EGLContext eglContext;

    EGLSurface eglSurface;

    EGLDisplay eglDisplay;

    GLuint currentProgram;
};
#endif //MYOPENGL2_RENDERHOLDER_H
