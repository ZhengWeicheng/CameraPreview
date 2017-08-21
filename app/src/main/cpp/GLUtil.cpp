
#include "GLUtil.h"
#include "log.h"
#include "RenderHolder.h"
#include <stdlib.h>
#include <android/native_window_jni.h>
#include <GLES2/gl2ext.h>

#define LOG_TAG "GLUtil"

#define GET_ARRAY_LEN(array, len){len = (sizeof(array) / sizeof(array[0]));}

void checkGlError(const char *op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
}

GLuint loadShader(GLenum shaderType, const char *shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc((size_t) infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char *pVertexSrc, const char *pFragmentSrc) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSrc);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSrc);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glBindAttribLocation(program, 0, "vPosition");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc((size_t) bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

void createRenderHolder(JNIEnv *env, jobject jSurface, RenderHolder *holder) {

    holder->eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (holder->eglDisplay == EGL_NO_DISPLAY) {
        checkGlError("eglGetDisplay");
        delete (&holder);
        return;
    }
    EGLint majorVer, minVer;
    if (!eglInitialize(holder->eglDisplay, &majorVer, &minVer)) {
        checkGlError("eglInitialize");
        LOGE("eglInitialize error");
        delete (&holder);
        return;
    }
    // EGL attributes
    const EGLint confAttr[] =
            {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,// very important!
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
        return;
    }
    ANativeWindow *surface = ANativeWindow_fromSurface(env, jSurface);
    holder->eglSurface = eglCreateWindowSurface(holder->eglDisplay, config, surface, NULL);
    if (holder->eglSurface == EGL_NO_SURFACE) {
        checkGlError("eglCreateWindowSurface");
        delete (&holder);
        return;
    }
    EGLint attrib_list[] =
            {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
    holder->eglContext = eglCreateContext(holder->eglDisplay, config, EGL_NO_CONTEXT, attrib_list);
    if (holder->eglContext == EGL_NO_CONTEXT) {
        checkGlError("eglCreateContext");
        delete (&holder);
        return;
    }


    if (!eglMakeCurrent(holder->eglDisplay, holder->eglSurface, holder->eglSurface, holder->eglContext)) {
        checkGlError("eglMakeCurrent");
        delete (&holder);
        return;
    }
}

GLuint* createExternalOESTexture() {
    GLuint *textures = new GLuint[1];
    glGenTextures(1, textures);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textures[0]);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return textures;
}

float d2r(float d) {
    return (float) (d / 180.0 * 3.141592653);
}