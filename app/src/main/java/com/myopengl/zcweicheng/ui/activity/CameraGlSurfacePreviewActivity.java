package com.myopengl.zcweicheng.ui.activity;

import android.app.Activity;
import android.os.Bundle;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.ui.widget.CameraGLSurfaceView;

/**
 * Created by zhengweicheng on 2017/8/17 0017.
 */

public class CameraGlSurfacePreviewActivity extends Activity {

    private CameraGLSurfaceView mCameraGlSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_glsurface_preview);
        mCameraGlSurfaceView = (CameraGLSurfaceView)findViewById(R.id.camera_preview);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mCameraGlSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCameraGlSurfaceView.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mCameraGlSurfaceView.destroy();
    }
}
