#include <jni.h>
#include <android/asset_manager.h>

//
// Created by Administrator on 2017/8/16 0016.
//
jmethodID getSurfaceUpdateImageMethodId(JNIEnv *env) {
    //输入纹理的方法
    jclass jcSurfaceTexture = env->FindClass("android/graphics/SurfaceTexture");
    return env->GetMethodID(jcSurfaceTexture, "updateTexImage", "()V");
}

char* getGLSLStr(AAssetManager* assetManager, const char* glslFileName) {
    AAsset *vertexAsset = AAssetManager_open(assetManager, glslFileName, AASSET_MODE_BUFFER);
    off_t length = AAsset_getLength(vertexAsset);
    char *vertexContent = new char[length + 1];
    AAsset_read(vertexAsset, vertexContent, (size_t) length);
    AAsset_close(vertexAsset);
    vertexContent[length] = '\0';
    return vertexContent;
}
