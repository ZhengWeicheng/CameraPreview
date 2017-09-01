//
// Created by Administrator on 2017/9/1 0001.
//

#ifndef MYOPENGL2_RECORDCAMERARENDER_H
#define MYOPENGL2_RECORDCAMERARENDER_H

#include "../RenderHolder.h"
#include "../filter/BaseFilter.h"
#include "../filter/RecordCameraFilter.h"


class RecordCameraRender : public RenderHolder {
public:
    RecordCameraRender() {
        RenderHolder();
        mRecordFilter = new RecordCameraFilter();
    }

    ~RecordCameraRender() {
        RenderHolder::~RenderHolder();
        if (mRecordFilter != NULL) {
            delete mRecordFilter;
            mRecordFilter = NULL;
        }
    }


public:
    RecordCameraFilter* mRecordFilter;

    GLuint mShareTextureId;
};
#endif //MYOPENGL2_RECORDCAMERARENDER_H
