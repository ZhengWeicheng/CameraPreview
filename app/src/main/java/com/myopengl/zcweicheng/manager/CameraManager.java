package com.myopengl.zcweicheng.manager;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.WindowManager;

import com.myopengl.zcweicheng.MyApp;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/**
 * Created by zhengweicheng on 2017/8/3 0003.
 */

public class CameraManager {
    private static final String TAG = "CameraManager";
    private static final int MSG_INIT = 1;
    private static final int MSG_STOP = 2;

    public static final int ERROR_PERMISSION_DENY = 1;
    public static final int ERROR_CAMERA_INITIALIZED = 2;
    public static final int ERROR_CAMERA_OPEN_ERROR = 3;

    public static final int MODE_PREVIEW_DATA = 1;
    public static final int MODE_PREVIEW_TEXTURE = 2;

    private CameraThread mCameraThread;
    private static final class CameraMangerHolder {
        static final CameraManager sINSTANCE = new CameraManager();
    }

    public static CameraManager getInstance() {
        return CameraMangerHolder.sINSTANCE;
    }

    private CameraManager() {
        mCameraThread = new CameraThread();
    }

    public void startCamera(Activity activity, int expectWidth, int expectHeight, int mode,
                            @NonNull CameraStateListener listener) {
        startCamera(activity, expectWidth, expectHeight, mode, null, listener);
    }

    public void startCamera(Activity activity, int expectWidth, int expectHeight, int mode,
                            SurfaceTexture outputTexture, @NonNull CameraStateListener listener) {
        if (ContextCompat.checkSelfPermission(activity, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            listener.onError(ERROR_PERMISSION_DENY);
            return;
        }
        Message.obtain(mCameraThread.mHandler, MSG_INIT,
                new Object[] {expectWidth, expectHeight, mode, outputTexture, listener}).sendToTarget();
    }

    public void stopCamera() {
        mCameraThread.exited = true;
        mCameraThread.mDataCallback = null;
        Message.obtain(mCameraThread.mHandler, MSG_STOP).sendToTarget();
    }

    public void setDataCallback(DataCallback dataCallback) {
        mCameraThread.mDataCallback = dataCallback;
    }

    public int getCameraIndex() {
        return mCameraThread != null ? mCameraThread.mCameraIndex : -1;
    }

    private static class CameraThread extends HandlerThread implements Handler.Callback, Camera.PreviewCallback {

        private Handler mHandler;
        private Camera mCamera = null;
        private int mWidth;
        private int mHeight;
        private int mCameraIndex;
        private ByteBuffer[] mVideoBuffer;
        private SurfaceTexture mSurfaceTexture;
        private int mVideoBufferIdx;
        private int mCameraOrientation;
        private int mZoomValue;
        private boolean isFocusing;
        private boolean exited;
        private DataCallback mDataCallback;
        private int mPreviewMode;

        public CameraThread() {
            super("camera thread");
            start();
            mCameraIndex = Camera.CameraInfo.CAMERA_FACING_BACK;
            mHandler = new Handler(Looper.myLooper(), this);
        }


        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_INIT:
                    initCamera(msg);
                    return true;
                case MSG_STOP:
                    release();
                    releaseObject();
                    return true;
            }
            return false;
        }

        private void initCamera(Message msg) {
            Log.w(TAG, "摄像头开始初始化");
            boolean init = false;
            Object[] objects = (Object[]) msg.obj;
            CameraStateListener listener = (CameraStateListener) objects[4];
            synchronized (this) {
                if (mCamera != null) {
                    Log.e(TAG, "initCamera 初始化摄像头时,摄像头已经初始化过了！");
                    listener.onError(ERROR_CAMERA_INITIALIZED);
                    return;
                }
                mCamera = null;
            }
            int expectedWidth = (int) objects[0];
            int expectedHeight = (int) objects[1];
            mPreviewMode = (int) objects[2];
            try {
                mCamera = Camera.open(mCameraIndex);
            } catch (Exception e) {
                e.printStackTrace();
                listener.onError(ERROR_CAMERA_OPEN_ERROR);
                return;
            }
            try {
                Camera.Parameters params = mCamera.getParameters();
                //格式
                params.setPreviewFormat(ImageFormat.NV21);
                //预览大小
                List<Camera.Size> sizes = params.getSupportedPreviewSizes();
                if (sizes != null) {
                    CameraSize frameSize = calculateCameraFrameSize(sizes, expectedWidth, expectedHeight);
                    Log.w(TAG, "预览设置为 " + frameSize.width + "x" + frameSize.height);
                    params.setPreviewSize(frameSize.width, frameSize.height);
                    mCamera.setParameters(params);
                }
                //帧率
                Camera.Parameters localParam = mCamera.getParameters();
                List<int[]> support = localParam.getSupportedPreviewFpsRange();
                if (support.size() > 0) {
                    int[] ints = support.get(0);
                    int max = Math.max(ints[0], ints[1]);
                    for (int[] size : support) {
                        max = Math.max(max, size[1]);
                    }
                    localParam.setPreviewFpsRange(max, max);
                    try {
                        mCamera.setParameters(localParam);
                        params = mCamera.getParameters();
                        Log.w(TAG, "帧率设置为:[" + max + "," + max + "]");
                    } catch (Exception e) {
                        Log.e(TAG, "WTF,不能设置为:[" + max + "," + max + "]，尝试设置为[" + ((int) (max * 0.9)) + "," + max + "]");
                        localParam = mCamera.getParameters();
                        try {
                            localParam.setPreviewFpsRange((int) (max * 0.9), max);
                            mCamera.setParameters(localParam);
                            params = mCamera.getParameters();
                            Log.w(TAG, "帧率设置为[" + max * 0.9 + "," + max + "]");
                        } catch (Exception e1) {
                            Log.e(TAG, "WTF,不能设置帧率");
                        }
                    }
                } else {
                    Log.e(TAG, "WTF,不能设置帧率");
                }
////                    优化
//                    if (!android.os.Build.MODEL.equals("GT-I9100")){
//                        params.setRecordingHint(true);
//                    }
                //聚焦
                List<String> focusModes = params.getSupportedFocusModes();
                if (focusModes != null) {
                    if (focusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                        params.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                    } else if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                        params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
                    } else if (focusModes.get(0) != null) {
                        params.setFocusMode(focusModes.get(0));
                    }
                    Log.d(TAG, Arrays.toString(focusModes.toArray()));
                }
//                相机缩放
                if(params.isZoomSupported()) {
                    mZoomValue = 0;
                    params.setZoom(mZoomValue);
                }
//                    params.setRecordingHint(true);
                mCamera.setParameters(params);
                exited = false;
                //记录宽高
                params = mCamera.getParameters();
                mWidth = params.getPreviewSize().width;
                mHeight = params.getPreviewSize().height;

                mSurfaceTexture = (SurfaceTexture) objects[3];
                if (mSurfaceTexture == null) {
                    mSurfaceTexture = new SurfaceTexture(1);
                }
                if (mPreviewMode == MODE_PREVIEW_DATA) {
                    int size = mWidth * mHeight;
                    size = size * ImageFormat.getBitsPerPixel(params.getPreviewFormat()) / 8;
                    mVideoBuffer = new ByteBuffer[2];
                    mVideoBuffer[0] = ByteBuffer.allocate(size);
                    mVideoBuffer[1] = ByteBuffer.allocate(size);
                    mCamera.addCallbackBuffer(mVideoBuffer[mVideoBufferIdx].array());
                    mCamera.setPreviewCallbackWithBuffer(this);
                }
                mCamera.setPreviewTexture(mSurfaceTexture);
                Log.d(TAG, params.flatten());
                mCamera.startPreview();
                updateCameraDegree();
                mHandler.removeCallbacks(mAutoFocusRunnable);
                mHandler.postDelayed(mAutoFocusRunnable, 1000);
                Log.w(TAG, "开始Camera预览");
                init = true;

            } catch (Exception e) {
                Log.e(TAG, "开启Camera失败！", e);
            }
            if (init) {
                Log.w(TAG, "摄像头初始化成功！");
            } else {
                exited = true;
                if (mCamera != null) {
                    release();
                }
                releaseObject();
                Log.w(TAG, "摄像头初始化失败！");
            }
            listener.onSuccess(mPreviewMode == MODE_PREVIEW_DATA ? null : mSurfaceTexture,
                    mWidth, mHeight);
        }

        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
//            Log.d(TAG, "=================onPreviewFrame========================= exited = ");

            //prepare another frame
            mVideoBufferIdx = 1 - mVideoBufferIdx;
            if (mCamera != null) {
                mCamera.addCallbackBuffer(mVideoBuffer[mVideoBufferIdx].array());
            }
            if (!exited) {
                if (mDataCallback != null) {
                    mDataCallback.callback(data, mWidth, mHeight, mCameraOrientation, mCameraIndex);
                }
            }
        }

        private void release() {
            synchronized (this) {
                try {
                    if (mCamera != null) {
                        mCamera.stopPreview();
                        mCamera.setPreviewCallback(null);
                        mCamera.release();
                    }
                    mCamera = null;
                    Log.w(TAG, "摄像机已释放");
                } catch (Exception e) {
                    Log.e(TAG, "stopCamera", e);
                }
            }
        }

        private void releaseObject() {
            synchronized (this) {
                if (mHandler != null) {
                    mHandler.removeCallbacks(mAutoFocusRunnable);
                    mHandler.removeCallbacksAndMessages(null);
                }
                if (mVideoBuffer != null) {
                    mVideoBuffer[0] = null;
                    mVideoBuffer[1] = null;
                    mVideoBuffer = null;
                }
                if (mSurfaceTexture != null) {
                    mSurfaceTexture.release();
                    mSurfaceTexture = null;
                }
            }
        }

        private void updateCameraDegree() {
            WindowManager wm = (WindowManager) MyApp.getContext().getSystemService(Context.WINDOW_SERVICE);
            setCameraDegreeByWindowRotation(wm.getDefaultDisplay().getRotation());
        }

        private void setCameraDegreeByWindowRotation(int windowRotation) {
            int mWindowDegree = 0;
            switch (windowRotation) {
                case Surface.ROTATION_0:
                    mWindowDegree = 0;
                    break;
                case Surface.ROTATION_90:
                    mWindowDegree = 90;
                    break;
                case Surface.ROTATION_180:
                    mWindowDegree = 180;
                    break;
                case Surface.ROTATION_270:
                    mWindowDegree = 270;
                    break;
            }
            if (mCamera != null) {
                Camera.CameraInfo info = new Camera.CameraInfo();
                Camera.getCameraInfo(mCameraIndex, info);

                if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
                    mCameraOrientation = (info.orientation + mWindowDegree) % 360;
                } else { // back-facing
                    mCameraOrientation = (info.orientation - mWindowDegree + 360) % 360;
                }
                mCameraOrientation = (360 - mCameraOrientation) % 360; // compensate the mirror
                mCamera.setDisplayOrientation(mCameraOrientation);
                Log.w(TAG, "摄像机角度 = " + mCameraOrientation);
            }
        }

        /**
         * 根据期望选择合适宽高
         */
        private CameraSize calculateCameraFrameSize(List<Camera.Size> supportedSizes, int expectWidth, int expectHeight) {
            int calcWidth = 0;
            int calcHeight = 0;

            for (Camera.Size size : supportedSizes) {
                int width = size.width;
                int height = size.height;
                Log.w(TAG, "摄像头支持Size:" + width + "x" + height + ", height/width=" + (float) height / width);

                if (width <= expectWidth && height <= expectHeight) {
                    if (width >= calcWidth && height >= calcHeight) {
                        calcWidth = width;
                        calcHeight = height;
                    }
                }
            }
            Log.w(TAG, "期望的Size:" + expectWidth + "x" + expectHeight + ",width/height=" + (float) expectWidth / expectHeight);

            return new CameraSize(calcWidth, calcHeight);
        }

        private void doTouchFocus(MotionEvent event) {
            if (mCamera == null || isFocusing) {
                return;
            }
            Log.d(TAG, "doTouchFocus");
            try {
                Camera.Parameters param = mCamera.getParameters();
                setFocusAndMeterAreas(param, getFocusAreas(event));
                mCamera.setParameters(param);
                isFocusing = true;
                mCamera.autoFocus(mAutoFocusCallback);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        private void setFocusAndMeterAreas(Camera.Parameters parameters, Rect rect) {
            if (rect == null) {
                rect = new Rect(-50, -50, 50, 50);
            }
            if (parameters.getMaxNumFocusAreas() > 0) {
                parameters.setFocusAreas(Collections.singletonList(new Camera.Area(rect, 600)));
            }
            if (parameters.getMaxNumMeteringAreas() > 0) {
                parameters.setMeteringAreas(Collections.singletonList(new Camera.Area(rect, 800)));
            }

        }

        private Rect getFocusAreas(MotionEvent event) {
            if (event == null) {
                return null;
            }
            Rect rect = new Rect();
            int screenWidth = Resources.getSystem().getDisplayMetrics().widthPixels;
            int screenHeight = Resources.getSystem().getDisplayMetrics().heightPixels;
            int centerX = (int) (event.getX() / screenWidth*2000 - 1000);
            int centerY = (int) (event.getY() / screenHeight*2000 - 1000);

            rect.left = clamp(centerX - 50, -1000, 1000);
            rect.top = clamp(centerY - 50, -1000, 1000);
            rect.right = clamp(centerX + 50, -1000, 1000);
            rect.bottom = clamp(centerY + 50, -1000, 1000);
            return rect;
        }

        private int clamp(int x, int min, int max) {
            if (x <= min) {
                x = min;
            } else if (x >= max) {
                x = max;
            }
            return x;
        }

        private Camera.AutoFocusCallback mAutoFocusCallback = new Camera.AutoFocusCallback() {

            @Override
            public void onAutoFocus(boolean arg0, Camera arg1) {
                isFocusing = false;
                try {
                    if (arg0 && mCamera != null) {
                        mCamera.cancelAutoFocus();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        };

        private Runnable mAutoFocusRunnable = new Runnable() {
            @Override
            public void run() {
                doTouchFocus(null);
            }
        };

        private class CameraSize {
            int width;
            int height;

            CameraSize(int width, int height) {
                this.width = width;
                this.height = height;
            }
        }

    }

    public interface CameraStateListener {
        /**
         * 初始化相机失败
         * @param errorType 错误类型
         */
        void onError(int errorType);

        /**
         * @param surfaceTexture 作为相机预览的texture
         * @param width 相机的预览宽度
         * @param height 相机预览的高度
         */
        void onSuccess(SurfaceTexture surfaceTexture, int width, int height);
    }

    /**
     * 当以相机previewCallback的形式预览时候，用该回调回调数据
     */
    public interface DataCallback{
        void callback(byte[] data, int mWidth, int mHeight, int mCameraOrientation, int mCameraIndex);
    }
}
