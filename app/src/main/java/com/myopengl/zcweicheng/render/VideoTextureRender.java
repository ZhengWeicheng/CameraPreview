package com.myopengl.zcweicheng.render;

import android.graphics.SurfaceTexture;
import android.os.Message;
import android.support.annotation.NonNull;
import android.view.Surface;

import static com.myopengl.zcweicheng.render.VideoTextureRenderThread.MSG_INIT;
import static com.myopengl.zcweicheng.render.VideoTextureRenderThread.MSG_RELEASE;
import static com.myopengl.zcweicheng.render.VideoTextureRenderThread.MSG_SET_SCALE;

/**
 * Created by zhengweicheng on 2017/8/1 0001.
 */

public class VideoTextureRender {

    private VideoTextureRenderThread mVideoTextureRenderThread;

    public VideoTextureRender(Surface outSurface, int width, int height, @NonNull VideoTextureRenderListener listener) {
        mVideoTextureRenderThread = new VideoTextureRenderThread();
        Message.obtain(mVideoTextureRenderThread.getHandler(),
                MSG_INIT, new Object[] {outSurface, width, height, listener}).sendToTarget();
    }

    public void setScale(boolean isScale) {
        Message.obtain(mVideoTextureRenderThread.getHandler(), MSG_SET_SCALE, isScale).sendToTarget();
    }

    public void release() {
        Message.obtain(mVideoTextureRenderThread.getHandler(), MSG_RELEASE).sendToTarget();
    }

    public interface VideoTextureRenderListener {
        void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height);
    }

}