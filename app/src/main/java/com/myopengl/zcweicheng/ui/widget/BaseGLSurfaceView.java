package com.myopengl.zcweicheng.ui.widget;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.myopengl.zcweicheng.Utils.Rotation;
import com.myopengl.zcweicheng.Utils.TextureRotationUtil;
import com.myopengl.zcweicheng.filter.BaseFilter;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by zhengweicheng on 2017/8/17 0017.
 */

public class BaseGLSurfaceView extends GLSurfaceView implements GLSurfaceView.Renderer {

    public static final int CENTER_INSIDE = 0, CENTER_CROP = 1, FIT_XY = 2;

    protected BaseFilter mFilter;
    protected int mSurfaceWidth;
    protected int mSurfaceHeight;
    protected int mPreviewWidth;
    protected int mPreviewHeight;
    protected int mScaleType = FIT_XY;//显示类型

    public BaseGLSurfaceView(Context context) {
        this(context, null);
    }

    public BaseGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        getHolder().addCallback(this);

    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glDisable(GL10.GL_DITHER);
        GLES20.glClearColor(0, 0, 0, 0);
        GLES20.glEnable(GL10.GL_CULL_FACE);
        GLES20.glEnable(GL10.GL_DEPTH_TEST);
        if (mFilter == null) {
            mFilter = new BaseFilter("none_vertex.glsl", "none_fragment.glsl");
            mFilter.init(getContext());
        }
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        mSurfaceWidth = width;
        mSurfaceHeight = height;
        onFilterChanged();
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
    }

    protected void onFilterChanged() {
        if (mFilter != null) {
            mFilter.initFrameBuffer(mPreviewWidth, mPreviewHeight);
            mFilter.onInputSizeChanged(mPreviewWidth, mPreviewHeight);
            mFilter.onDisplaySizeChanged(mSurfaceWidth, mSurfaceHeight);
        }
    }

//    protected void adjustSize(int rotation, boolean flipHorizontal, boolean flipVertical) {
//        float[][] data = adjustSize(mSurfaceWidth, mSurfaceHeight, rotation,
//                flipHorizontal, flipVertical);
//
////        mFilter.updateBuffer(data);
//    }

    /**
     * 调整画面大小
     *
     * @param width          宽
     * @param height         高
     * @param rotation       角度
     * @param flipHorizontal 是否水平翻转
     * @param flipVertical   是否垂直翻转
     */
    protected float[][] adjustSize(int width, int height, int rotation, boolean flipHorizontal, boolean flipVertical) {
        float[] textureCords = TextureRotationUtil.getRotation(Rotation.fromInt(rotation),
                flipHorizontal, flipVertical);
        float[] cube = TextureRotationUtil.CUBE;
        float ratio1 = (float) width / mPreviewWidth;
        float ratio2 = (float) height / mPreviewHeight;
        float ratioMax = Math.max(ratio1, ratio2);
        int imageWidthNew = Math.round(mPreviewWidth * ratioMax);
        int imageHeightNew = Math.round(mPreviewHeight * ratioMax);

        float ratioWidth = imageWidthNew / (float) width;
        float ratioHeight = imageHeightNew / (float) height;

        switch (mScaleType) {
            case CENTER_INSIDE:
                cube = new float[]{
                        TextureRotationUtil.CUBE[0] / ratioHeight, TextureRotationUtil.CUBE[1] / ratioWidth,
                        TextureRotationUtil.CUBE[2] / ratioHeight, TextureRotationUtil.CUBE[3] / ratioWidth,
                        TextureRotationUtil.CUBE[4] / ratioHeight, TextureRotationUtil.CUBE[5] / ratioWidth,
                        TextureRotationUtil.CUBE[6] / ratioHeight, TextureRotationUtil.CUBE[7] / ratioWidth,
                };
                break;
            case CENTER_CROP:
                float distHorizontal = (1 - 1 / ratioWidth) / 2;
                float distVertical = (1 - 1 / ratioHeight) / 2;
                textureCords = new float[]{
                        addDistance(textureCords[0], distVertical), addDistance(textureCords[1], distHorizontal),
                        addDistance(textureCords[2], distVertical), addDistance(textureCords[3], distHorizontal),
                        addDistance(textureCords[4], distVertical), addDistance(textureCords[5], distHorizontal),
                        addDistance(textureCords[6], distVertical), addDistance(textureCords[7], distHorizontal),
                };
                break;
            case FIT_XY:

                break;
        }
        return new float[][]{cube, textureCords};
    }

    private float addDistance(float coordinate, float distance) {
        return coordinate == 0.0f ? distance : 1 - distance;
    }
}
