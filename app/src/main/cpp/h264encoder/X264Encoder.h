//
// Created by Administrator on 2017/9/1 0001.
//

#ifndef MYOPENGL2_X264ENCODER_H
#define MYOPENGL2_X264ENCODER_H
#include <stdio.h>
extern "C" {
#include "x264.h"
#include "x264_config.h"
}

enum bitrate_lev
{
    BIT_HIGH_LEVEL = 0,
    BIT_STANDARD_LEVEL = 1,
    BIT_MEDIUM_LEVEL = 2,
    BIT_LOW_LEVEL = 3,
};

class X264Encoder {
public:
    X264Encoder() {
        x264Param = NULL;
        x264Encoder = NULL;
        mInPic = NULL;
        mOutPic = NULL;
        pictureBuffer = NULL;
        bitratelevel = BIT_HIGH_LEVEL;
    }

    ~X264Encoder() {
        if (x264Param != NULL) {
            delete x264Param;
            x264Param = NULL;
        }
        if (x264Encoder != NULL) {
            delete x264Encoder;
            x264Encoder = NULL;
        }

        if (mInPic != NULL) {
            delete mInPic;
            mInPic = NULL;
        }

        if (mOutPic != NULL) {
            delete mOutPic;
            mOutPic = NULL;
        }
        if (pictureBuffer != NULL) {
            delete pictureBuffer;
            pictureBuffer = NULL;
        }
        x264Encoder = NULL;
        mInPic = NULL;
        mOutPic = NULL;
        pictureBuffer = NULL;
        bitratelevel = BIT_HIGH_LEVEL;
    }

public :
    x264_param_t * x264Param;
    x264_t       * x264Encoder;
    x264_picture_t * mInPic;
    x264_picture_t *mOutPic;
    uint8_t * pictureBuffer;
    unsigned int bitratelevel;

private :
    int mWidth;
    int mHeight;
    int mFps;

public :
    void initX264Encode(int width, int height, int fps, int bite);
    void startEncoder(uint8_t * dataptr,char *&bufdata,int &buflen, int &isKeyFrame);
    void releaseEncoder();

private :
    void Flush();
};



#endif //MYOPENGL2_X264ENCODER_H
