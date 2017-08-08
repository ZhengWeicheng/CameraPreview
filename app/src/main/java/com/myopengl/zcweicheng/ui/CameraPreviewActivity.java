package com.myopengl.zcweicheng.ui;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.manager.CameraManager;
import com.myopengl.zcweicheng.manager.CameraManager.CameraStateListener;
import com.myopengl.zcweicheng.render.CameraTextureRender;
import com.myopengl.zcweicheng.render.CameraTextureRender.CameraTextureRenderListener;

import static com.myopengl.zcweicheng.manager.CameraManager.MODE_PREVIEW_TEXTURE;

/**
 * Created by zhengweicheng on 2017/8/4 0004.
 *
 */

public class CameraPreviewActivity extends Activity implements TextureView.SurfaceTextureListener {

    private CameraTextureRender mRender;
    private float startX, diff;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview);

        TextureView textureView = (TextureView) findViewById(R.id.preview_texture);
        textureView.setSurfaceTextureListener(this);
        textureView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        startX = event.getX();
                        break;
                    case MotionEvent.ACTION_MOVE:
                        diff = event.getX() - startX;
                        float distance = diff/720;
                        if (mRender != null) {
                            mRender.setDiff(distance);
                        }
                        break;
                    case MotionEvent.ACTION_UP:
                        break;
                }
                return true;
            }
        });
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mRender = new CameraTextureRender(new Surface(surface), width, height, new CameraTextureRenderListener() {
            @Override
            public void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height) {
                CameraManager.getInstance().startCamera(CameraPreviewActivity.this,
                        640, 480, MODE_PREVIEW_TEXTURE, surfaceTexture, new CameraStateListener() {
                            @Override
                            public void onError(int errorType) {

                            }

                            @Override
                            public void onSuccess(SurfaceTexture surfaceTexture,
                                                  int width, int height) {
                                mRender.setCameraSize(width, height);
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
