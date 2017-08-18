package com.myopengl.zcweicheng.ui.widget;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.myopengl.zcweicheng.Utils.CameraHelper;
import com.myopengl.zcweicheng.Utils.OpenGlUtils;
import com.myopengl.zcweicheng.filter.CameraInputFilter;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by zhengweicheng on 2017/8/17 0017.
 *
 */

public class CameraGLSurfaceView extends BaseGLSurfaceView implements GLSurfaceView.Renderer {

    public static final int RECORD_WIDTH = 480, RECORD_HEIGHT = 640;
    private final CameraHelper mCameraHelper;

    private int mTextureId;
    private SurfaceTexture mSurfaceTexture;
    private CameraInputFilter mCameraInputFilter;

    public CameraGLSurfaceView(Context context) {
        this(context, null);

    }

    public CameraGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mTextureId = OpenGlUtils.NO_TEXTURE;
        mCameraHelper = new CameraHelper();
    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        super.onSurfaceCreated(gl, config);

        if (mCameraInputFilter == null) {
            mCameraInputFilter = new CameraInputFilter();
            mCameraInputFilter.init(getContext());
        }
//        if (mRecordFilter == null) {
//            mRecordFilter = new RecordFilter();
//            mRecordFilter.init(getContext());
//            mRecordFilter.setRecordListener(mOnRecordListener);
//        }

        if (mTextureId == OpenGlUtils.NO_TEXTURE) {
            mTextureId = OpenGlUtils.getExternalOESTextureID();
            if (mTextureId != OpenGlUtils.NO_TEXTURE) {
                mSurfaceTexture = new SurfaceTexture(mTextureId);
                mSurfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
                    @Override
                    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                        requestRender();
                    }
                });
            }
            mCameraHelper.startPreview(mSurfaceTexture);
        }

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        super.onSurfaceChanged(gl, width, height);

        CameraHelper.CameraItem info = mCameraHelper.getCameraAngleInfo();
        float[][] data = adjustSize(mSurfaceWidth, mSurfaceHeight, info.orientation,
                info.isFront, !info.isFront);

        mCameraInputFilter.updateBuffer(data);

        //重新计算录制顶点、纹理坐标
//        float[][] data = adjustSize(mRecordWidth, mRecordHeight, info.orientation,
//                info.isFront, !info.isFront);
//        mRecordCubeBuffer.clear();
//        mRecordCubeBuffer.put(data[0]).position(0);
//        mRecordTextureBuffer.clear();
//        mRecordTextureBuffer.put(data[1]).position(0);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        super.onDrawFrame(gl);
        if (mSurfaceTexture == null)
            return;
        mSurfaceTexture.updateTexImage();
        float[] mtx = new float[16];
        mSurfaceTexture.getTransformMatrix(mtx);

        //先将纹理绘制到fbo同时过滤镜
        mFilter.setTextureTransformMatrix(mtx);
        int id = mFilter.onDrawToTexture(mTextureId);
//
//        //绘制到屏幕上
        mCameraInputFilter.onDrawFrame(id);

        //绘制到另一个fbo上，同时使用pbo获取数据
//        mRecordFilter.onDrawToFbo(id, mRecordCubeBuffer, mRecordTextureBuffer,
//                mSurfaceTexture.getTimestamp());
    }

    @Override
    protected void onFilterChanged() {
        super.onFilterChanged();

        mCameraInputFilter.onDisplaySizeChanged(mSurfaceWidth, mSurfaceHeight);

        //初始化fbo，pbo
//        mRecordFilter.initFrameBuffer(mRecordWidth, mRecordHeight);
//        mRecordFilter.initPixelBuffer(mRecordWidth, mRecordHeight);
//        mRecordFilter.onInputSizeChanged(mRecordWidth, mRecordHeight);
//        mRecordFilter.onDisplaySizeChanged(mSurfaceWidth, mSurfaceHeight);
    }

    @Override
    public void onResume() {
        super.onResume();
        boolean rel = mCameraHelper.openCamera();
        if (rel) {
            review();
            if (mSurfaceTexture != null)
                mCameraHelper.startPreview();
        } else {
//            mThreadHelper.sendError("摄像头开启失败，请检查是否被占用！");
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        mCameraHelper.stopCamera();
    }


    public void destroy() {
        mCameraHelper.stopCamera();
        mFilter.destroy();
        mCameraInputFilter.destroy();
//        mRecordFilter.destroy();
    }

    //调整view大小
    private void review() {
        mPreviewWidth = mCameraHelper.getPreviewWidth();
        mPreviewHeight = mCameraHelper.getPreviewHeight();
//        mRecordWidth = mCameraHelper.getRecordWidth();
//        mRecordHeight = mCameraHelper.getRecordHeight();
    }
}
