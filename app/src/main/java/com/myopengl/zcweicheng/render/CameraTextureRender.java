package com.myopengl.zcweicheng.render;

import android.graphics.SurfaceTexture;
import android.os.Message;
import android.support.annotation.NonNull;
import android.view.Surface;

import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_INIT;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_RELEASE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_CAMERA_SIZE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_DISTANCE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_SCALE;


/**
 * Created by zhengweicheng on 2017/8/7 0007.
 */

public class CameraTextureRender {
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

    public void setDiff(float distance) {
        Message.obtain(mCameraTextureThread.getHandler(), MSG_SET_DISTANCE,
                distance).sendToTarget();
    }

    public interface CameraTextureRenderListener {
        void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height);
    }
}
