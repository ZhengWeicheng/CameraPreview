package com.myopengl.zcweicheng.ui.activity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.gles.ContentManager;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ContentManager.initialize(this);

        ContentManager cm = ContentManager.getInstance();
        if (!cm.isContentCreated(this)) {
            ContentManager.getInstance().createAll(this);
        }

        Button button1 = (Button) findViewById(R.id.play_video);
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, PlayVideoActivity.class));
            }
        });

        Button button2 = (Button) findViewById(R.id.camera_preview);
        button2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, CameraPreviewActivity.class));
            }
        });

        Button button3 = (Button) findViewById(R.id.camera_preview_data);
        button3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, CameraDataPreviewActivity.class));
            }
        });

        Button button4 = (Button) findViewById(R.id.test_animator);
        button4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, TestAnimatorDrawableActivity.class));
            }
        });

        Button button5 = (Button) findViewById(R.id.gl_surface_preview);
        button5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startActivity(new Intent(MainActivity.this, CameraGlSurfacePreviewActivity.class));
            }
        });


    }

}
