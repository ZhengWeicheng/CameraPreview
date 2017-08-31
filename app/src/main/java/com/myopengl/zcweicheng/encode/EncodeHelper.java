package com.myopengl.zcweicheng.encode;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;


import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * 录制数据
 *
 * @author Created by jz on 2017/5/2 11:21
 */
public class EncodeHelper {

    private static final int MAX_CACHE_BUFFER_NUMBER = 24;

    private List<ByteBuffer> mBuffers;
    private List<byte[]> mReusableBuffers;
    private byte[] bytes;
    private Thread mThread;
    private boolean isRecording;

    public EncodeHelper() {
        isRecording = false;
        mBuffers = Collections.synchronizedList(new ArrayList<ByteBuffer>());
//        mReusableBuffers = Collections.synchronizedList(new ArrayList<byte[]>());
    }

    public void onRecord(ByteBuffer buffer, int size) {
        if (mBuffers.size() >= MAX_CACHE_BUFFER_NUMBER) {
            Log.e("EncodeHelper", "mBuffers size >= MAX_CACHE_BUFFER_NUMBER 12");
            return;
        }

//        byte[] data = getBuffer(size);
//        buffer.get(data);
//        buffer.clear();
        mBuffers.add(buffer);
    }

    public void start() {
        if (mThread != null) {
            return;
        }
        mThread = new MyThread();
        mThread.start();
        isRecording = true;
    }

    public void stop() {
        isRecording = false;
    }

    private byte[] getBuffer(int length) {
        if (mReusableBuffers.isEmpty()) {
            return new byte[length];
        } else {
            return mReusableBuffers.remove(0);
        }
    }
    private class MyThread extends Thread {//转换成Bitmap演示用效率低下，可以用libyuv代替

        @Override
        public void run() {
            while (isRecording || !mBuffers.isEmpty()) {
                if (mBuffers.isEmpty()) {
                    SystemClock.sleep(1);
                    continue;
                }
                ByteBuffer buffer = mBuffers.remove(0);
                if (bytes == null) {
                    bytes = new byte[buffer.capacity()];
                }
                buffer.get(bytes);
                FFmpegBridge.encodeFrame2H264(bytes);
            }
            mBuffers.clear();
        }
    }
}