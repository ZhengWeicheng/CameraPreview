1.视频解码
使用MediaExtractor MediaCodec解码MP4文件渲染到TextureView SurfaceTexture生成的Surface上。
  渲染的过程中可以添加滤镜以及opengl的其他渲染。

  a.TextureView->SurfaceTexture->Surface 作为outputSurface
  b.VideoTextureRenderThread使用outputSurface创建GL环境，然后创建输入Surface inputSurface
  c.使用inputSurface 创建MediaExtractor 和 MeidaCodec 解码器，解码到inputSurface
  d.渲染然到outputSurface

2.相机预览(预览都是通过C++代码实现，提高效率)

  a.YUV数据预览，不需要在cpu中进行格式转换，直接opengl渲染
    camera->onPreviewFrame->opengl渲染->display camera
    CameraDataPreviewActivity.java

    mPreviewTexture.setSurfaceTextureListener(this);

     @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            mRender = Renderer.prepare(new Surface(surface), width, height);
            startCamera();
        }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        stopCamera();
        return true;
    }

    private void startCamera() {
        CameraManager.getInstance().startCamera(this, 640, 480, CameraManager.MODE_PREVIEW_DATA,
                new CameraManager.CameraStateListener() {
                    @Override
                    public void onError(int errorType) {

                    }

                    @Override
                    public void onSuccess(SurfaceTexture surfaceTexture, int width, int height) {

                    }
                });
        CameraManager.getInstance().setDataCallback(new CameraManager.DataCallback() {
            @Override
            public void callback(byte[] data, int mWidth, int mHeight, int mCameraOrientation,
                                 int mCameraIndex) {
                if (mRender != null) {
                    mRender.process(data, mWidth ,mHeight, mCameraOrientation,
                            mCameraIndex == Camera.CameraInfo.CAMERA_FACING_FRONT, 0);
                }
            }
        });
    }

    private void stopCamera() {
        CameraManager.getInstance().stopCamera();
        if (mRender != null) {
            mRender.release();
        }
    }

  b.Texture预览，创建opengl的surfaceTexture给相机预览，拷贝surfaceTexture数据到显示的TextureView时，进行opengl滤镜

    CameraPreviewActivity.java

    textureView.setSurfaceTextureListener(this);

     @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            mRender = new CameraTextureRender(new Surface(surface), width, height, new CameraTextureRenderListener() {
                @Override
                public void onInputTextureCreate(SurfaceTexture surfaceTexture, int width, int height) {
                    CameraManager.getInstance().startCamera(CameraPreviewActivity.this,
                            640, 480, MODE_PREVIEW_TEXTURE, surfaceTexture, new CameraStateListener() {
                                @Override
                                public void onError(int errorType) {

                                }

                                @Override
                                public void onSuccess(SurfaceTexture surfaceTexture,
                                                      int width, int height) {
                                    mRender.setCameraSize(width, height);
                                }
                            });
                }
            });
            mRender.setScale(true);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            CameraManager.getInstance().stopCamera();
            if (mRender != null) {
                mRender.release();
            }
            return true;
        }


