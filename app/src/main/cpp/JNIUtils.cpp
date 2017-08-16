#include <jni.h>

//
// Created by Administrator on 2017/8/16 0016.
//
jmethodID getSurfaceUpdateImageMethodId(JNIEnv *env) {
    //输入纹理的方法
    jclass jcSurfaceTexture = env->FindClass("android/graphics/SurfaceTexture");
    return env->GetMethodID(jcSurfaceTexture, "updateTexImage", "()V");
}
