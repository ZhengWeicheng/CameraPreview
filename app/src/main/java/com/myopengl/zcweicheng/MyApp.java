package com.myopengl.zcweicheng;

import android.app.Application;
import android.content.Context;

/**
 * Created by zhengweicheng on 2017/8/2 0002.
 */

public class MyApp extends Application {

    public static Context context;

    public static Context getContext() {
        return context;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        context = this;
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
    }
}
