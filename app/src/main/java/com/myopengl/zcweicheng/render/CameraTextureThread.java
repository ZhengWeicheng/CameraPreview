package com.myopengl.zcweicheng.render;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import com.myopengl.zcweicheng.gles.EglCore;
import com.myopengl.zcweicheng.gles.FullFrameRect;
import com.myopengl.zcweicheng.gles.Texture2dProgram;
import com.myopengl.zcweicheng.gles.WindowSurface;
import com.myopengl.zcweicheng.manager.CameraManager;

/**
 * Created by zhengweicheng on 2017/8/7 0007.
 */

public class CameraTextureThread extends HandlerThread implements Handler.Callback, SurfaceTexture.OnFrameAvailableListener {

    public static final int MSG_INIT = 1;
    public static final int MSG_RELEASE = 2;
    public static final int MSG_SET_SCALE = 3;
    private static final String TAG = "RenderThread";

    private Handler mHandler;
    private Surface mOutputSurface;
    private int mWidth;
    private int mHeight;
    private EglCore mEglCore;
    private WindowSurface mDisplaySurface;
    private FullFrameRect mFullFrameBlit;
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
        Matrix.rotateM(mverMatrix, 0, 180, 0.0f, 0.0f, 1.0f);
    }

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
        }
        return false;
    }

    private void init(Message msg) {
        Object[] objects = (Object[]) msg.obj;
        mOutputSurface = (Surface) objects[0];
        mWidth = (int) objects[1];
        mHeight = (int) objects[2];
        mCameraIndex = CameraManager.getInstance().getCameraIndex();
        mListener = (CameraTextureRender.CameraTextureRenderListener) objects[3];
        mEglCore = new EglCore(null, EglCore.FLAG_RECORDABLE);
        mDisplaySurface = new WindowSurface(mEglCore, mOutputSurface, true);
        mDisplaySurface.makeCurrent();
        mFullFrameBlit = new FullFrameRect(
                new Texture2dProgram(Texture2dProgram.ProgramType.TEXTURE_EXT));
        mTextureId = mFullFrameBlit.createTextureObject();
        mInputTexture = new SurfaceTexture(mTextureId);
        mInputTexture.setOnFrameAvailableListener(this);
        mListener.onInputTextureCreate(mInputTexture, mWidth, mHeight);
    }

    private void release() {
        mListener = null;
        isScale = false;
        if (mInputTexture != null) {
            mInputTexture.release();
            mInputTexture = null;
        }
        if (mDisplaySurface != null) {
            mDisplaySurface.release();
            mDisplaySurface = null;
        }
        if (mFullFrameBlit != null) {
            mFullFrameBlit.release(false);
            mFullFrameBlit = null;
        }
        if (mEglCore != null) {
            mEglCore.release();
            mEglCore = null;
        }
        Log.d(TAG, "release");
    }


    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
//        Log.d(TAG, "onFrameAvailable mWidth " + mWidth + " mHeight = " + mHeight);
        if (mEglCore == null) {
            Log.d(TAG, "Skipping drawFrame after shutdown");
            return;
        }
        // Latch the next frame from the camera.
        mDisplaySurface.makeCurrent();
        mInputTexture.updateTexImage();
        mInputTexture.getTransformMatrix(mTmpMatrix);
        GLES20.glViewport(0, 0, mWidth, mHeight);
        if (isScale) {
            Matrix.scaleM(mTmpMatrix, 0, 1f*480/720, 1f, 1f);
        }
        mFullFrameBlit.drawFrame(mTextureId, mverMatrix, mTmpMatrix);
        mDisplaySurface.swapBuffers();

        mFrameNum++;
    }
}

