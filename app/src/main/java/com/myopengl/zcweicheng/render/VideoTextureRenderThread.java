package com.myopengl.zcweicheng.render;

import android.graphics.SurfaceTexture;
import android.opengl.GLES20;
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
import com.myopengl.zcweicheng.render.VideoTextureRender.VideoTextureRenderListener;

/**
 * Created by zhengweicheng on 2017/8/1 0001.
 */

public class VideoTextureRenderThread extends HandlerThread implements Handler.Callback, SurfaceTexture.OnFrameAvailableListener {

    public static final int MSG_INIT = 1;
    public static final int MSG_RELEASE = 2;
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
    private VideoTextureRenderListener mListener;

    public VideoTextureRenderThread() {
        super("VideoTextureRenderThread");
        start();
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
        }
        return false;
    }

    private void init(Message msg) {
        Object[] objects = (Object[]) msg.obj;
        mOutputSurface = (Surface) objects[0];
        mWidth = (int) objects[1];
        mHeight = (int) objects[2];
        mListener = (VideoTextureRenderListener) objects[3];
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
        Log.d(TAG, "onFrameAvailable mWidth " + mWidth + " mHeight = " + mHeight);
        if (mEglCore == null) {
            Log.d(TAG, "Skipping drawFrame after shutdown");
            return;
        }
        // Latch the next frame from the camera.
        mDisplaySurface.makeCurrent();
        mInputTexture.updateTexImage();
        mInputTexture.getTransformMatrix(mTmpMatrix);
        GLES20.glViewport(0, 0, mWidth, mHeight);
        mFullFrameBlit.drawFrame(mTextureId, mTmpMatrix);
        drawExtra(mFrameNum, mWidth, mHeight);
        mDisplaySurface.swapBuffers();

        mFrameNum++;
    }

    private void drawExtra(int frameNum, int width, int height) {
        // We "draw" with the scissor rect and clear calls.  Note this uses window coordinates.
        int val = frameNum % 3;
        switch (val) {
            case 0:  GLES20.glClearColor(1.0f, 0.0f, 0.0f, 1.0f);   break;
            case 1:  GLES20.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);   break;
            case 2:  GLES20.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);   break;
        }

        int xpos = (int) (width * ((frameNum % 100) / 100.0f));
        GLES20.glEnable(GLES20.GL_SCISSOR_TEST);
        GLES20.glScissor(xpos, 0, width / 10, height / 10);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glDisable(GLES20.GL_SCISSOR_TEST);
    }
}
