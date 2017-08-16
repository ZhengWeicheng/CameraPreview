package com.myopengl.zcweicheng.ui;

import android.support.annotation.NonNull;
import android.view.MotionEvent;
import android.view.View;

import com.myopengl.zcweicheng.MyApp;

/**
 * Created by zhengweicheng on 2017/8/14 0014.
 *
 */

public class TextureTouchListener implements View.OnTouchListener {

    private onUpdateListener mListener;
    private float startX, diff, distance;
    private int mWindowWidth;
    private int mCurrentFilterId = 0, mNextFilterId = 0;
    private UpdateHandler mUpdateHandlerThread;
    private boolean isChnagingFilter;

    public interface onUpdateListener {
        void onUpdate(float distance, int filterId);
    }

    public TextureTouchListener(@NonNull  onUpdateListener listener) {
        mListener = listener;
        mWindowWidth = MyApp.getContext().getResources().getDisplayMetrics().widthPixels;
        mUpdateHandlerThread = new UpdateHandler();
        isChnagingFilter = false;
    }


    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (isChnagingFilter) {
            return false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                startX = event.getX();
                break;
            case MotionEvent.ACTION_MOVE:
                diff = event.getX() - startX;
                if ((diff < 0 && mCurrentFilterId == 4) || (diff > 0 && mCurrentFilterId == 0)) {
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
                    distance = (mWindowWidth - diff)/mWindowWidth;
                } else {
                    distance = diff/mWindowWidth;
                }
                setDiff(distance, mNextFilterId);
                break;
            case MotionEvent.ACTION_UP:
                if ((diff < 0 && mCurrentFilterId == 5) || (diff > 0 && mCurrentFilterId == 0)) {
                    return false;
                }
                isChnagingFilter = true;
                mUpdateHandlerThread.mHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (Math.abs(diff) > mWindowWidth * 0.2) {
                            if (diff > 0 && distance > 0) {
                                distance = distance - 0.05f;
                                if (distance < 0) {
                                    distance = 0;
                                }
                                setDiff(distance, mNextFilterId);
                                mUpdateHandlerThread.mHandler.postDelayed(this, 10);
                            } else if (diff <= 0 && distance > -1) {
                                distance = distance - 0.05f;
                                if (distance < -1) {
                                    distance = -1;
                                }
                                setDiff(distance, mNextFilterId);
                                mUpdateHandlerThread.mHandler.postDelayed(this, 10);
                            } else {
                                mCurrentFilterId = mNextFilterId;
                                isChnagingFilter = false;
                            }
                        } else  {
                            if (diff > 0 && distance < 1) {
                                distance = distance + 0.05f;
                                if (distance > 1) {
                                    distance = 1;
                                }
                                setDiff(distance, mNextFilterId);
                                mUpdateHandlerThread.mHandler.postDelayed(this, 10);
                            } else if (diff <= 0 && distance < 0) {
                                distance = distance + 0.02f;
                                if (distance >= 0) {
                                    distance = 0;
                                    setDiff(distance, mCurrentFilterId);
                                } else {
                                    setDiff(distance, mNextFilterId);
                                }
                                mUpdateHandlerThread.mHandler.postDelayed(this, 10);
                            } else {
                                mNextFilterId = mCurrentFilterId;
                                isChnagingFilter = false;
                            }
                        }
                    }
                }, 30);
                break;
        }
        return true;
    }

    private void setDiff(float distance, int nextFilterId) {
        if (mListener != null) {
            mListener.onUpdate(distance, nextFilterId);
        }
    }
}
