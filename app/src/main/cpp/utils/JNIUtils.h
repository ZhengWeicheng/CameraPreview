//
// Created by Administrator on 2017/8/16 0016.
//

#ifndef MYOPENGL2_JNIUTILS_H
#define MYOPENGL2_JNIUTILS_H

#include <jni.h>
#include <android/asset_manager.h>

jmethodID getSurfaceUpdateImageMethodId(JNIEnv *env);
char* getGLSLStr(AAssetManager* assetManager, const char* glslFileName);
#endif //MYOPENGL2_JNIUTILS_H
