package com.myopengl.zcweicheng.render;

import android.content.res.AssetManager;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import com.myopengl.zcweicheng.MyApp;
import com.myopengl.zcweicheng.Utils.AssetsUtils;
import com.myopengl.zcweicheng.gles.EglCore;
import com.myopengl.zcweicheng.gles.FullFrameRect;
import com.myopengl.zcweicheng.gles.Texture2dProgram;
import com.myopengl.zcweicheng.gles.WindowSurface;
import com.myopengl.zcweicheng.manager.CameraManager;

/**
 * Created by zhengweicheng on 2017/8/7 0007.
 */

public class CameraTextureThread extends HandlerThread implements Handler.Callback, SurfaceTexture.OnFrameAvailableListener {

    private static final String TAG = "RenderThread";

    static final int MSG_INIT = 1;
    static final int MSG_RELEASE = 2;
    static final int MSG_SET_SCALE = 3;
    static final int MSG_SET_CAMERA_SIZE = 4;
    static final int MSG_SET_DISTANCE = 5;
    static final int MSG_SET_FILTER_ID = 6;

    private Handler mHandler;
    private Surface mOutputSurface;
    private int mWidth;
    private int mHeight;
    private int mTextureId;
    private SurfaceTexture mInputTexture;
    private final float[] mTmpMatrix = new float[16];
    private int mFrameNum;
    private boolean isScale;
    private CameraTextureRender.CameraTextureRenderListener mListener;
    private int mCameraIndex;

    public static final float[] mverMatrix;
    static {
        mverMatrix = new float[16];
        Matrix.setIdentityM(mverMatrix, 0);
    }

    private int mCameraSurfaceWith;
    private int mCameraSurfaceHeight;
    private float mDistance;
    private float mFilterId;
    private long enginId;

    public CameraTextureThread() {
        super("VideoTextureRenderThread");
        start();
        isScale = false;
        mHandler = new Handler(Looper.myLooper(), this);
    }

    public Handler getHandler() {
        return mHandler;
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
            case MSG_SET_SCALE:
                isScale = (boolean) msg.obj;
                return true;
            case MSG_SET_CAMERA_SIZE:
                Object[] objects = (Object[]) msg.obj;
                mCameraSurfaceWith = (int)objects[0];
                mCameraSurfaceHeight = (int)objects[1];
                return true;
            case MSG_SET_DISTANCE:
                Object[] objects1 = (Object[]) msg.obj;
                mDistance = (float) objects1[0];
                mFilterId = (float) objects1[1];
                return true;
            case MSG_SET_FILTER_ID:
                mFilterId = (float) msg.obj;
                return true;
        }
        return false;
    }

    private void init(Message msg) {
        Object[] objects = (Object[]) msg.obj;
        mOutputSurface = (Surface) objects[0];
        String vertex = AssetsUtils.getFromAssets("vertex.glsl");
        String fragment = AssetsUtils.getFromAssets("slide_fragment.glsl");
        mWidth = (int) objects[1];
        mHeight = (int) objects[2];
        enginId = nativeInit(mOutputSurface, MyApp.getContext().getAssets(), mWidth, mHeight);
        if (enginId == 0) {
            Log.d("aaaaaaa","初始化出错啦");
            return;
        }

        mTextureId = nativeGetInputTex(enginId);
        mInputTexture = new SurfaceTexture(mTextureId);
        mInputTexture.setOnFrameAvailableListener(this);

        mListener = (CameraTextureRender.CameraTextureRenderListener) objects[3];

        mCameraIndex = CameraManager.getInstance().getCameraIndex();

        rotateCameraMatrix();

        mListener.onInputTextureCreate(mInputTexture, mWidth, mHeight);
    }

    public void rotateCameraMatrix() {
        if (mCameraIndex == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            Matrix.rotateM(mverMatrix, 0, 0, 0.f, 0.f, 1.f);
        } else {
            Matrix.rotateM(mverMatrix, 0, 180, 0.f, 0.f, 1.f);
        }
    }

    private void release() {
        mListener = null;
        isScale = false;
        if (mInputTexture != null) {
            mInputTexture.release();
            mInputTexture = null;
        }
        nativeRelease(enginId);
        Log.d(TAG, "release");
    }


    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (enginId == 0 || mInputTexture == null) {
            Log.d(TAG, "Skipping drawFrame after shutdown");
            return;
        }
        // Latch the next frame from the camera.
        mInputTexture.getTransformMatrix(mTmpMatrix);
        if (isScale) {
            Matrix.scaleM(mTmpMatrix, 0, 1f*mCameraSurfaceWith/mWidth, 1f, 1f);
        }
        nativeDraw(surfaceTexture, mverMatrix, mTmpMatrix, mDistance, mFilterId, enginId);
        mFrameNum++;
    }

    public void startEncode() {
        if (enginId == 0) {
            Log.d(TAG, "START ENCODE ERROR");
            return;
        }
        startRecord(enginId, Environment.getExternalStorageDirectory().getAbsolutePath(),
                String.valueOf(System.currentTimeMillis()), 0, mWidth, mHeight, mWidth, mHeight, 20, 1000000);
    }

    public void stopEncode() {
        if (enginId == 0) {
            Log.d(TAG, "stopEncode ERROR");
            return;
        }
        stopRecord(enginId);
    }

//    private static native long nativeInit(Surface surface, String vertex, String fragment);

    private native long nativeInit(Surface output, AssetManager assets, int outWidth, int outHeight);
    private static native void nativeRelease(long holder);
    private static native void nativeDraw(SurfaceTexture surface, float[] mverMatrix, float[] mTmpMatrix,
                                          float distance, float nextFilterId, long holder);

    /**
     *  录制结束
     * @return
     */
    public static native int stopRecord(long enginId);

    /**
     *
     * @param mediaBasePath 视频存放目录
     * @param mediaName 视频名称
     * @param filter 旋转镜像剪切处理
     * @param in_width 输入视频宽度
     * @param in_height 输入视频高度
     * @param out_height 输出视频高度
     * @param out_width 输出视频宽度
     * @param frameRate 视频帧率
     * @param bit_rate 视频比特率
     * @return
     */
    public static native int startRecord(long enginId, String mediaBasePath, String mediaName,
            int filter,int in_width, int in_height, int out_width, int  out_height, int frameRate, long bit_rate);


    private static native int nativeGetInputTex(long engine);
    private static native long nativeGetEglContext(long engine);

}

