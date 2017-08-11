package com.myopengl.zcweicheng.ui;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
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
    private float startX, diff, lastDiff, distance;
    Handler handler;
    private int mWindowWidth;
    private int mCurrentFilterId = 0, mNextFilterId = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview);

        TextureView textureView = (TextureView) findViewById(R.id.preview_texture);
        textureView.setSurfaceTextureListener(this);
        handler = new Handler();
        mWindowWidth = getResources().getDisplayMetrics().widthPixels;
        textureView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        startX = event.getX();
                        handler.removeCallbacksAndMessages(null);
                        break;
                    case MotionEvent.ACTION_MOVE:
                        diff = event.getX() - startX + lastDiff;
                        if ((diff < 0 && mCurrentFilterId == 5) || (diff > 0 && mCurrentFilterId == 0)) {
                            return false;
                        }
                        if ((diff > 0 && mCurrentFilterId == 5)) {
                            mNextFilterId = 4;
                        } else if ((diff < 0 && mCurrentFilterId == 0)) {
                            mNextFilterId = 1;
                        } else {
                            mNextFilterId = diff > 0 ? mCurrentFilterId - 1 : mCurrentFilterId + 1;
                            if (mNextFilterId > 5) {
                                mNextFilterId = 5;
                            } else if (mNextFilterId < 0) {
                                mNextFilterId = 0;
                            }
                        }
                        if (diff > 0) {
                            diff = mWindowWidth - diff;
                        }

                        distance = diff/mWindowWidth;

                        Log.d("aaa" , "diff = " + diff + " mNextFilterId = " + mNextFilterId
                                + " distance = " + distance);


                        if (mRender != null) {
                            mRender.setDiff(distance, mNextFilterId);
                        }
                        break;
                    case MotionEvent.ACTION_UP:
                        if ((diff < 0 && mCurrentFilterId == 5) || (diff > 0 && mCurrentFilterId == 0)) {
                            return false;
                        }
                        lastDiff = diff;
                        handler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                if (Math.abs(diff) > mWindowWidth * 0.7) {
                                    if (diff > 0 && distance < 1) {
                                        distance = distance + 0.05f;
                                        if (distance > 1) {
                                            distance = 1;
                                        }
                                        Log.d("aaa" , "diff = " + diff + " mNextFilterId = " + mNextFilterId
                                                + " distance = " + distance);
                                        mRender.setDiff(distance, mNextFilterId);
                                        handler.postDelayed(this, 50);
                                    } else if (diff <= 0 && distance > -1) {
                                        distance = distance - 0.05f;
                                        if (distance < -1) {
                                            distance = -1;
                                        }
                                        Log.d("aaa" , "diff = " + diff + " mNextFilterId = " + mNextFilterId
                                                + " distance = " + distance);
                                        mRender.setDiff(distance, mNextFilterId);
                                        handler.postDelayed(this, 50);
                                    } else {
                                        lastDiff = 0;
                                        mCurrentFilterId = mNextFilterId;
                                    }
                                } else if (Math.abs(diff) < mWindowWidth * 0.3) {
                                    if (diff > 0 && distance > 0) {
                                        distance = distance - 0.05f;
                                        if (distance < 0) {
                                            distance = 0;
                                        }
                                        Log.d("aaa" , "diff = " + diff + " mNextFilterId = " + mNextFilterId
                                                + " distance = " + distance);
                                        mRender.setDiff(distance, mNextFilterId);
                                        handler.postDelayed(this, 50);
                                    } else if (diff <= 0 && distance < 0) {
                                        distance = distance + 0.05f;
                                        if (distance > 1) {
                                            distance = 1;
                                        }
                                        Log.d("aaa" , "diff = " + diff + " mNextFilterId = " + mNextFilterId
                                                + " distance = " + distance);
                                        mRender.setDiff(distance, mNextFilterId);
                                        handler.postDelayed(this, 50);
                                    } else {
                                        lastDiff = 0;
                                        mCurrentFilterId = mNextFilterId;
                                    }
                                }
                            }
                        }, 50);
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
