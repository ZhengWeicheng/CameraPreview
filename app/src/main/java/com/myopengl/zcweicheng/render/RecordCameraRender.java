package com.myopengl.zcweicheng.render;

import android.content.res.AssetManager;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.support.v7.view.menu.MenuWrapperFactory;

import com.myopengl.zcweicheng.MyApp;
import com.myopengl.zcweicheng.encode.FFmpegBridge;

import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_INIT;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_PROCESS;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_RELEASE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_CAMERA_SIZE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_DISTANCE;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_FILTER_ID;
import static com.myopengl.zcweicheng.render.CameraTextureThread.MSG_SET_SCALE;

/**
 * Created by zhengweicheng on 2017/9/1 0001.
 */

public class RecordCameraRender {

    static {
        System.loadLibrary("avutil");
        System.loadLibrary("fdk-aac");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("swscale");
        System.loadLibrary("swresample");
        System.loadLibrary("avfilter");
//        System.loadLibrary("x264");
        System.loadLibrary("native-lib");
    }
    private RecordCameraRenderThread mRecordCameraRenderThread;

    public RecordCameraRender(int width, int height, int shareTextureId, long shareContext, Object locker) {
        mRecordCameraRenderThread = new RecordCameraRenderThread();
        Message.obtain(mRecordCameraRenderThread.getHandler(),
                MSG_INIT, new Object[] {width, height, shareTextureId, shareContext, locker}).sendToTarget();
    }

    public void setScale(boolean isScale) {
        Message.obtain(mRecordCameraRenderThread.getHandler(), MSG_SET_SCALE, isScale).sendToTarget();
    }

    public void release() {
        Message.obtain(mRecordCameraRenderThread.getHandler(), MSG_RELEASE).sendToTarget();
    }

    public void setCameraSize(int width, int height) {
        Message.obtain(mRecordCameraRenderThread.getHandler(), MSG_SET_CAMERA_SIZE,
                new Object[] {width, height}).sendToTarget();
    }

    public void setDiff(float distance, float filterId) {
        Message.obtain(mRecordCameraRenderThread.getHandler(), MSG_SET_DISTANCE,
                new Object[] {distance, filterId}).sendToTarget();
    }

    public void setFilterId(float filterId) {
        Message.obtain(mRecordCameraRenderThread.getHandler(), MSG_SET_FILTER_ID,
                filterId).sendToTarget();
    }

    public void process() {
        Message.obtain(mRecordCameraRenderThread.getHandler(), MSG_PROCESS).sendToTarget();
    }

    private static class RecordCameraRenderThread extends HandlerThread implements Handler.Callback {

        Handler handler;
        private int width;
        private int height;
        private int mShareTextureId;
        private long mShareContext;
        private Object mLocker;
        private long mEngin;
        private boolean exist;

        public RecordCameraRenderThread() {
            super("RecordCameraRenderThread");
            start();
            handler = new Handler(Looper.myLooper(), this);

        }

        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_INIT:
                    init(msg);
                    return true;
                case MSG_RELEASE:
                    release();
                    return true;
                case MSG_PROCESS:
                    process();
                    return true;
            }
            return false;
        }

        private void process() {
            if (exist) {
                return;
            }
            synchronized (mLocker) {
                nativeProcess(mEngin);
            }
        }

        private void release() {
            exist = true;
            nativeRelease(mEngin);
            FFmpegBridge.recordEnd();
        }



        private void init(Message msg) {
            Object[] objects = (Object[]) msg.obj;
            width = (int) objects[0];
            height = (int) objects[1];
            mShareTextureId = (int) objects[2];
            mShareContext = (long) objects[3];
            mLocker = objects[4];

            mEngin = nativeInit(width, height, mShareTextureId, mShareContext, MyApp.getContext().getAssets());
            if (mEngin == 0) {
                return;
            }
            exist = false;
            FFmpegBridge.prepareJXFFmpegEncoder(Environment.getExternalStorageDirectory().getAbsolutePath(),
                    String.valueOf(System.currentTimeMillis()), 0, width, height, width, height, 30, 500000);
        }


        public Handler getHandler() {
            return handler;
        }
    }

    private static native long nativeInit(int width, int height, int shareTextureId, long shareContext, AssetManager assets);
    private static native void nativeRelease(long engine);
    private static native void nativeProcess(long engine);
}
