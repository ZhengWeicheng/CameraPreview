package com.myopengl.zcweicheng.ui;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

/**
 * Created by zhengweicheng on 2017/8/16 0016.
 */

public class UpdateHandler extends HandlerThread implements Handler.Callback {

    public Handler mHandler;

    public UpdateHandler() {
        super("UpdateHandler");
        start();
        mHandler = new Handler(getLooper(), this);
    }

    @Override
    public boolean handleMessage(Message msg) {
        return false;
    }
}