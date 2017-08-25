package com.myopengl.zcweicheng.render;

import android.graphics.SurfaceTexture;
import android.os.Message;
import android.support.annotation.NonNull;
import android.view.Surface;

import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_INIT;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_RELEASE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_CAMERA_SIZE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_DISTANCE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_FILTER_ID;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_SCALE;


/**
 * Created by zhengweicheng on 2017/8/7 0007.
 */

public class CameraTextureRender {
    static {
        System.loadLibrary("avutil");
        System.loadLibrary("fdk-aac");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("swscale");
        System.loadLibrary("swresample");
        System.loadLibrary("avfilter");
        System.loadLibrary("native-lib");
    }
    private CameraTextureThread mCameraTextureThread;

    public CameraTextureRender(Surface outSurface, int width, int height, @NonNull CameraTextureRenderListener listener) {
        mCameraTextureThread = new CameraTextureThread();
        Message.obtain(mCameraTextureThread.getHandler(),
                MSG_INIT, new Object[] {outSurface, width, height, listener}).sendToTarget();
    }

    public void setScale(boolean isScale) {
        Message.obtain(mCameraTextureThread.getHandler(), MSG_SET_SCALE, isScale).sendToTarget();
    }

    public void release() {
        Message.obtain(mCameraTextureThread.getHandler(), MSG_RELEASE).sendToTarget();
    }

    public void setCameraSize(int width, int height) {
        Message.obtain(mCameraTextureThread.getHandler(), MSG_SET_CAMERA_SIZE,
                new Object[] {width, height}).sendToTarget();
    }

    public void setDiff(float distance, float filterId) {
        Message.obtain(mCameraTextureThread.getHandler(), MSG_SET_DISTANCE,
                new Object[] {distance, filterId}).sendToTarget();
    }

    public void setFilterId(float filterId) {
        Message.obtain(mCameraTextureThread.getHandler(), MSG_SET_FILTER_ID,
                filterId).sendToTarget();
    }

    public void startRecord() {
        mCameraTextureThread.startEncode();
    }

    public void stopRecord() {
        mCameraTextureThread.stopEncode();
    }

    public void rotateCameraMatrix() {
        mCameraTextureThread.rotateCameraMatrix();
    }

    public interface CameraTextureRenderListener {
        void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height);
    }
}
