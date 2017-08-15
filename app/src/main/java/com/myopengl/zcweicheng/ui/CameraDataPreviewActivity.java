package com.myopengl.zcweicheng.ui;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.manager.CameraManager;
import com.myopengl.zcweicheng.render.Renderer;
import com.myopengl.zcweicheng.ui.TextureTouchListener.onUpdateListener;

/**
 * Created by zhengweicheng on 2017/8/14 0014.
 */

public class CameraDataPreviewActivity extends Activity implements TextureView.SurfaceTextureListener {
    TextureView mPreviewTexture;

    private Renderer mRender;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview_data);

        mPreviewTexture = (TextureView) findViewById(R.id.preview_texture);

        mPreviewTexture.setSurfaceTextureListener(this);

        mPreviewTexture.setOnTouchListener(new TextureTouchListener(new onUpdateListener() {
            @Override
            public void onUpdate(float distance, int filterId) {
                if (mRender != null) {
                    mRender.setFilterDiff(distance, filterId);
                }
            }
        }));
    }

    @Override
    protected void onResume() {
        super.onResume();
        startCamera();

    }

    @Override
    protected void onPause() {
        super.onPause();
        stopCamera();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mRender = Renderer.prepare(new Surface(surface), width, height);
        startCamera();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        stopCamera();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }

    private void startCamera() {
        CameraManager.getInstance().startCamera(this, 640, 480, CameraManager.MODE_PREVIEW_DATA,
                new CameraManager.CameraStateListener() {
                    @Override
                    public void onError(int errorType) {

                    }

                    @Override
                    public void onSuccess(SurfaceTexture surfaceTexture, int width, int height) {

                    }
                });
        CameraManager.getInstance().setDataCallback(new CameraManager.DataCallback() {
            @Override
            public void callback(byte[] data, int mWidth, int mHeight, int mCameraOrientation, int mCameraIndex) {
                if (mRender != null) {
                    mRender.process(data, mWidth ,mHeight, mCameraOrientation,
                            mCameraIndex == Camera.CameraInfo.CAMERA_FACING_FRONT, 0);
                }
            }
        });
    }

    private void stopCamera() {
        CameraManager.getInstance().stopCamera();
        if (mRender != null) {
            mRender.release();
        }
    }

}
