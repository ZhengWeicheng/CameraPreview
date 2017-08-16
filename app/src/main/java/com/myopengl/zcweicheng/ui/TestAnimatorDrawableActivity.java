package com.myopengl.zcweicheng.ui;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.myopengl.zcweicheng.R;
import com.myopengl.zcweicheng.ui.widget.AnimatedRingDrawable;

/**
 * Created by zhengweicheng on 2017/8/15 0015.
 */

public class TestAnimatorDrawableActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animator);
        TextView textView = (TextView) findViewById(R.id.animator);

        final AnimatedRingDrawable drawable = new AnimatedRingDrawable();
        drawable.setInvertTransformation(true);
        drawable.setDuration(1000);
        drawable.setFromScale(0.9f);
        drawable.setMaxInnerCircleR(dp2px(90));
        drawable.setMaxOutCircleR(dp2px(100));
        drawable.setOutCircleR(dp2px(60));
        drawable.setMinOutCircleR(dp2px(60));
        textView.setBackgroundDrawable(drawable);

        Button button = (Button) findViewById(R.id.start_animator);
        Button button1 = (Button) findViewById(R.id.stop_animator);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                drawable.start();
            }
        });
        button1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                drawable.stop();
            }
        });
    }

    private int dp2px(int dp) {
        return (int) (getResources().getDisplayMetrics().density * dp);
    }
}
