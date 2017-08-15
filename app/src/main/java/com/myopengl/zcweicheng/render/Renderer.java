package com.myopengl.zcweicheng.render;

import android.app.Notification;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.ImageFormat;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.Process;
import android.util.Log;
import android.view.Surface;

import com.myopengl.zcweicheng.Utils.AssetsUtils;

/**
 * Created by iFinVer on 2016/11/27.
 * ilzq@foxmail.com
 */

public class Renderer {

    static {
        System.loadLibrary("native-lib");
    }

    private static final String TAG = "Renderer";
    private static int sFinEngineCount = 0;

    public static final int FILTER_TYPE_NORMAL = 0; //must be zero here.
    public static final int FILTER_TYPE_GREY_SCALE = 1;
    public static final int FILTER_TYPE_SEPIA_STONE = 2;
    public static final int FILTER_TYPE_CYAN = 3;
    public static final int FILTER_TYPE_RADIAL_BLUR = 4;
    public static final int FILTER_TYPE_NEGATIVE_COLOR = 5;
    public static final int FILTER_TYPE_V_MIRROR = 6;
    public static final int FILTER_TYPE_H_MIRROR = 7;
    public static final int FILTER_TYPE_FISH_EYE = 8;

    private final RendThread mEngineThread;
    private final int mEngineId;

    private Renderer(Surface output, int width, int height) {
        mEngineThread = new RendThread(output, width, height);
        mEngineId  = ++sFinEngineCount;
    }

    public static Renderer prepare(Surface output, int width, int height) {
        return new Renderer(output, width, height);
    }

    public void process(byte[] data, int frameWidth, int frameHeight, int degree, boolean isFrontCamera, long facePtr) {
        mEngineThread.process(data, frameWidth, frameHeight, degree, isFrontCamera,facePtr);
    }

    public void release() {
        mEngineThread.release();
    }

    public void resizeInput(int surfaceWidth, int surfaceHeight) {
        mEngineThread.resizeInput(surfaceWidth, surfaceHeight);
    }

    public void switchFilter(Context ctx, int filterType) {
        mEngineThread.switchFilter(ctx, filterType);
    }

    public void setFilterDiff(float distance, int nextFilterId) {
        mEngineThread.mDistance = distance;
        mEngineThread.mNextFilterId = nextFilterId;
    }

    public int getCurrentFilter() {
        return mEngineThread.mFilterType;
    }

    private class RendThread extends HandlerThread implements Handler.Callback {
        private final int MSG_INIT = 1;
        private final int MSG_RELEASE = 2;
        private final int MSG_PROCESS = 3;
        private final int MSG_SWITCH_FILTER = 4;

        private Handler mHandler;
        private Surface mOutputSurface;
        private boolean isPrepared;
        private int mFrameWidth;
        private int mFrameHeight;
        private byte[] mData;
        private int mDegree;
        private boolean isFrontCamera;
        private int mOutWidth;
        private int mOutHeight;
        private AssetManager mAssetManager;
        private int mFilterType;
        private long mEngine;
        private long mFacePtr;

        private int mNextFilterId;
        private float mDistance;

        RendThread(Surface output, int width, int height) {
            super("RendThread", Process.THREAD_PRIORITY_URGENT_DISPLAY);
            this.mFilterType = FILTER_TYPE_NORMAL;
            this.mOutputSurface = output;
            this.mOutWidth = width;
            this.mOutHeight = height;
            start();
            mHandler = new Handler(getLooper(), this);
            Log.w(TAG, "Renderer"+mEngineId+"开始初始化");
            mHandler.sendEmptyMessage(MSG_INIT);
        }


        public void switchFilter(Context ctx, int filterType) {
            synchronized (RendThread.class) {
                this.mAssetManager = ctx.getApplicationContext().getAssets();
                this.mFilterType = filterType;
            }
            mHandler.sendEmptyMessage(MSG_SWITCH_FILTER);
        }

        public void process(byte[] data, int frameWidth, int frameHeight, int degree, boolean isFrontCamera, long facePtr) {
            this.mFrameWidth = frameWidth;
            this.mFrameHeight = frameHeight;
            this.mData = data;
            this.mDegree = degree;
            this.isFrontCamera = isFrontCamera;
            this.mFacePtr = facePtr;
            if (mHandler != null) {
                mHandler.sendEmptyMessage(MSG_PROCESS);
            }
        }

        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_INIT:
                    init();
                    return true;
                case MSG_RELEASE:
                    nativeRelease(mEngine);
                    Log.w(TAG, "Renderer"+mEngineId+"已释放");
                    sFinEngineCount --;
                    isPrepared = false;
                    return true;
                case MSG_SWITCH_FILTER:
                    switchFilterInternal();
                    return true;
                case MSG_PROCESS:
                    if (isPrepared && mData != null) {
                        nativeRender(mEngine,mData, mFrameWidth, mFrameHeight, mDegree,
                                isFrontCamera, mOutWidth, mOutHeight,mDistance, mNextFilterId, mFacePtr);
                    }
                    return true;
            }
            return false;
        }

        private void switchFilterInternal() {
//            if (isPrepared) {
//                Log.w(TAG, "Renderer"+mEngineId+"开始切换滤镜");
//                synchronized (RendThread.class) {
//                    FinFiltersManager.Shader shader = FinFiltersManager.findShader(mFilterType);
//                    nativeSwitchFilter(mEngine,mAssetManager, mFilterType, shader.vertex, shader.fragment);
//                }
//            }
        }

        private void init() {
            String vertex = AssetsUtils.getFromAssets("data_preview_vertex.glsl");
            String fragment = AssetsUtils.getFromAssets("data_preview_fragment.glsl");
            mEngine = nativeInit(mOutputSurface, vertex, fragment);
            isPrepared = mEngine != 0;
            if (isPrepared) {
                Log.w(TAG, "Renderer"+mEngineId+"初始化成功");
            } else {
                Log.e(TAG, "Renderer"+mEngineId+"初始化失败!");
                sendReleaseMsg();
            }
        }

        private void sendReleaseMsg() {
            if (!isPrepared) {
                return;
            }
            isPrepared = false;
            mHandler.sendEmptyMessage(MSG_RELEASE);
        }

        public void release() {
            Log.w(TAG, "Renderer"+mEngineId+"开始释放");
            sendReleaseMsg();
        }

        public void resizeInput(int surfaceWidth, int surfaceHeight) {
            this.mOutWidth = surfaceWidth;
            this.mOutHeight = surfaceHeight;
        }
    }


    /**
     * @return 0 means failed
     */
    private native long nativeInit(Surface output, String vertex, String fragment);

    private native void nativeRelease(long engine);

    private native void nativeSwitchFilter(long engine, AssetManager mAssetManager, int mFilterType, String mVertexName, String mFragmentName);

    private native void nativeRender(long engine,
                                     byte[] data,
                                     int frameWidth,
                                     int frameHeight,
                                     int degree,
                                     boolean mirror,
                                     int mOutWidth,
                                     int mOutHeight,
                                     float distance,
                                     int nextFilterId,
                                     long mFacePtr);
}
