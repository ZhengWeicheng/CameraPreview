//
// Created by Administrator on 2017/8/16 0016.
//

#ifndef MYOPENGL2_TEXTURERENDER_H
#define MYOPENGL2_TEXTURERENDER_H

#include <jni.h>
#include "../RenderHolder.h"

class TextureRenderHolder : public RenderHolder {
public:
    TextureRenderHolder() {
        RenderHolder();
        textures = NULL;
    }

    ~TextureRenderHolder() {
        if (textures != NULL) {
            glDeleteTextures(textureNums, textures);
        }
    }
public:
    GLuint posAttrVertices;

    GLuint posAttrTexCoords;

    GLuint posDistance;

    GLuint posNextFilterId;

    int textureNums;

    GLuint *textures;

    GLuint posVertex;

    GLuint posTexMat;

    jmethodID updatImageMethodId;
};

const GLfloat VERTICES_RENDER[] =
{
    -1.0f, 1.0f,//pos0
    -1.0f, -1.0f,//pos1
    1.0f, -1.0f,//pos2
    1.0f, 1.0f,//pos3

};

const GLfloat TEXTURE_RENDER[] =
{
        1.0f, 0.0f,//tex0
        1.0f, 1.0f,//tex1
        0.0f, 1.0f,//tex2
        0.0f, 0.0f,//tex3

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
