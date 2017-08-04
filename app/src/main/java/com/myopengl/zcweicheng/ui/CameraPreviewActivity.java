package com.myopengl.zcweicheng.ui;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.view.Surface;
import android.view.TextureView;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.manager.CameraManager;
import com.myopengl.zcweicheng.manager.CameraManager.CameraStateListener;
import com.myopengl.zcweicheng.render.VideoTextureRender;

import static com.myopengl.zcweicheng.manager.CameraManager.MODE_PREVIEW_TEXTURE;

/**
 * Created by zhengweicheng on 2017/8/4 0004.
 */

public class CameraPreviewActivity extends Activity implements TextureView.SurfaceTextureListener {

    private VideoTextureRender mRender;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview);

        TextureView textureView = (TextureView) findViewById(R.id.preview_texture);
        textureView.setSurfaceTextureListener(this);
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mRender = new VideoTextureRender(new Surface(surface), width, height, new VideoTextureRender.VideoTextureRenderListener() {
            @Override
            public void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height) {
                CameraManager.getInstance().startCamera(CameraPreviewActivity.this,
                        640, 480, MODE_PREVIEW_TEXTURE, surfaceTexture, new CameraStateListener() {
                            @Override
                            public void onError(int errorType) {

                            }

                            @Override
                            public void onSuccess(boolean isSuccess, SurfaceTexture surfaceTexture,
                                                  int width, int height) {

                            }
                        });
            }
        });
        mRender.setScale(true);
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {

    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        CameraManager.getInstance().stopCamera();
        if (mRender != null) {
            mRender.release();
        }
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {

    }
}
