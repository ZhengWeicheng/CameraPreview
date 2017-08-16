package com.myopengl.zcweicheng.ui.widget;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Rect;
import android.view.animation.AnimationUtils;

/**
 * Created by zhengweicheng on 2017/8/16 0016.
 */

public class AnimatedRingDrawable extends AnimatedScaleDrawable {
    public static final String  TAG = AnimatedRingDrawable.class.getSimpleName();
    private final Paint mPaint;
    private final PorterDuffXfermode mSrcOver;
    private final PorterDuffXfermode mDstOver;
    private int outCircleR;
    private int innerCircleR;
    private int maxOutCircleR;
    private int minOutCircleR;
    private int maxInnerCircleR;
    private int minInnerCircleR;
    public AnimatedRingDrawable() {
        super();
        mPaint = new Paint();
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setStrokeWidth(2);
        mPaint.setColor(Color.RED);
        mPaint.setAntiAlias(true);
        mSrcOver = new PorterDuffXfermode(PorterDuff.Mode.SRC_OVER);
        mDstOver = new PorterDuffXfermode(PorterDuff.Mode.DST_OUT);
        innerCircleR = minInnerCircleR = 0;
        outCircleR = minOutCircleR = 60;
        maxInnerCircleR = 80;
        maxOutCircleR = 100;
    }

    /**
     * 设置最大的外圆半径 px
     * @param maxOutCircleR
     */
    public void setMaxOutCircleR(int maxOutCircleR) {
        this.maxOutCircleR = maxOutCircleR;
    }

    /**
     * 设置最小的外圆半径 px
     * @param minOutCircleR
     */
    public void setMinOutCircleR(int minOutCircleR) {
        this.minOutCircleR = minOutCircleR;
    }

    /**
     * 设置最大的内圆半径 px
     * @param maxInnerCircleR
     */
    public void setMaxInnerCircleR(int maxInnerCircleR) {
        this.maxInnerCircleR = maxInnerCircleR;
    }

    /**
     * 设置当前外圆半径 px
     * @param outCircleR
     */
    public void setOutCircleR(int outCircleR) {
        this.outCircleR = outCircleR;
    }

    /**
     * 设置当前内圆半径 px
     * @param innerCircleR
     */
    public void setInnerCircleR(int innerCircleR) {
        this.innerCircleR = innerCircleR;
    }

    /**
     * 设置最小的内圆半径 px
     * @param minInnerCircleR
     */
    public void setMinInnerCircleR(int minInnerCircleR) {
        this.minInnerCircleR = minInnerCircleR;
    }

    private Runnable startAnimatorRunnable = new Runnable() {
        @Override
        public void run() {
            if (outCircleR == maxOutCircleR && innerCircleR == maxInnerCircleR) {
                animator();
            } else {
                innerCircleR += 2;
                outCircleR += 1;
                if (outCircleR > maxOutCircleR) {
                    outCircleR = maxOutCircleR;
                }
                if (innerCircleR > maxInnerCircleR) {
                    innerCircleR = maxInnerCircleR;
                }
            }

            invalidateSelf();
            scheduleSelf(this, 30);
        }
    };

    private Runnable stopAnimatorRunnable = new Runnable() {
        @Override
        public void run() {

            if (outCircleR == minOutCircleR && innerCircleR == minInnerCircleR) {
                invalidateSelf();
                unscheduleSelf(this);
            } else {
                innerCircleR -= 2;
                outCircleR -= 1;
                if (outCircleR < minOutCircleR) {
                    outCircleR = minOutCircleR;
                }
                if (innerCircleR < minInnerCircleR) {
                    innerCircleR = minInnerCircleR;
                }
                scheduleSelf(this, 30);
                invalidateSelf();
            }
        }
    };

    private void animator() {
        long animTime = AnimationUtils.currentAnimationTimeMillis();
        mState.mAnimation.getTransformation(animTime, mState.mTransformation);
        float transformation = mState.mTransformation.getAlpha();
        mState.mScale = (mState.mMinScale + (mState.mMaxScale - mState.mMinScale)
                * (mState.mInvert ? (1.0f - transformation) : transformation));
    }

    @Override
    public void start() {
        super.start();
        scheduleSelf(startAnimatorRunnable, 30);
    }

    @Override
    public void stop() {
        super.stop();
        unscheduleSelf(startAnimatorRunnable);
        scheduleSelf(stopAnimatorRunnable, 30);
    }

    @Override
    public void draw(Canvas canvas) {
        final Rect bounds = (mState.mUseBounds ? getBounds() : mTmpRect);

        int saveCount = canvas.save();

        int centerX = bounds.left + bounds.width()  / 2;
        int centerY = bounds.top  + bounds.height() / 2;
        mPaint.setXfermode(mSrcOver);
        mPaint.setColor(Color.RED);
        canvas.drawCircle(centerX, centerY, outCircleR, mPaint);
        mPaint.setXfermode(mDstOver);
        mPaint.setColor(Color.WHITE);
        canvas.drawCircle(centerX, centerY, innerCircleR == maxInnerCircleR
                ? innerCircleR * mState.mScale : innerCircleR, mPaint);

        canvas.restoreToCount(saveCount);
    }
}
