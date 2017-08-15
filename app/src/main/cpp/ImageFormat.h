//
// Created by Administrator on 2017/4/28 0028.
//

#ifndef FINENGINE_IMAGEFORMAT_H
#define FINENGINE_IMAGEFORMAT_H
/*
     * these constants are chosen to be binary compatible with their previous
     * location in PixelFormat.java
     */

#define UNKNOWN 0

/**
 * RGB format used for pictures encoded as RGB_565. See
 * {@link android.hardware.Camera.Parameters#setPictureFormat(int)}.
 */
#define RGB_565 4

/**
 * <p>Android YUV format.</p>
 *
 * <p>This format is exposed to software decoders and applications.</p>
 *
 * <p>YV12 is a 4:2:0 YCrCb planar format comprised of a WxH Y plane followed
 * by (W/2) x (H/2) Cr and Cb planes.</p>
 *
 * <p>This format assumes
 * <ul>
 * <li>an even width</li>
 * <li>an even height</li>
 * <li>a horizontal stride multiple of 16 pixels</li>
 * <li>a vertical stride equal to the height</li>
 * </ul>
 * </p>
 *
 * <pre> y_size = stride * height
 * c_stride = ALIGN(stride/2, 16)
 * c_size = c_stride * height/2
 * size = y_size + c_size * 2
 * cr_offset = y_size
 * cb_offset = y_size + c_size</pre>
 *
 * <p>For the {@link android.hardware.camera2} API, the {@link #YUV_420_888} format is
 * recommended for YUV output instead.</p>
 *
 * <p>For the older camera API, this format is guaranteed to be supported for
 * {@link android.hardware.Camera} preview images since API level 12; for earlier API versions,
 * check {@link android.hardware.Camera.Parameters#getSupportedPreviewFormats()}.
 *
 * <p>Note that for camera preview callback use (see
 * {@link android.hardware.Camera#setPreviewCallback}), the
 * <var>stride</var> value is the smallest possible; that is, it is equal
 * to:
 *
 * <pre>stride = ALIGN(width, 16)</pre>
 *
 * @see android.hardware.Camera.Parameters#setPreviewCallback
 * @see android.hardware.Camera.Parameters#setPreviewFormat
 * @see android.hardware.Camera.Parameters#getSupportedPreviewFormats
 * </p>
 */
#define YV12 0x32315659

/**
 * <p>Android Y8 format.</p>
 *
 * <p>Y8 is a YUV planar format comprised of a WxH Y plane only, with each pixel
 * being represented by 8 bits. It is equivalent to just the Y plane from {@link #YV12}
 * format.</p>
 *
 * <p>This format assumes
 * <ul>
 * <li>an even width</li>
 * <li>an even height</li>
 * <li>a horizontal stride multiple of 16 pixels</li>
 * </ul>
 * </p>
 *
 * <pre> y_size = stride * height </pre>
 *
 * <p>For example, the {@link android.media.Image} object can provide data
 * in this format from a {@link android.hardware.camera2.CameraDevice}
 * through a {@link android.media.ImageReader} object if this format is
 * supported by {@link android.hardware.camera2.CameraDevice}.</p>
 *
 * @see android.media.Image
 * @see android.media.ImageReader
 * @see android.hardware.camera2.CameraDevice
 *
 * @hide
 */
#define Y8  0x20203859

/**
 * <p>Android Y16 format.</p>
 *
 * Y16 is a YUV planar format comprised of a WxH Y plane, with each pixel
 * being represented by 16 bits. It is just like {@link #Y8}, but has 16
 * bits per pixel (little endian).</p>
 *
 * <p>This format assumes
 * <ul>
 * <li>an even width</li>
 * <li>an even height</li>
 * <li>a horizontal stride multiple of 16 pixels</li>
 * </ul>
 * </p>
 *
 * <pre> y_size = stride * height </pre>
 *
 * <p>For example, the {@link android.media.Image} object can provide data
 * in this format from a {@link android.hardware.camera2.CameraDevice}
 * through a {@link android.media.ImageReader} object if this format is
 * supported by {@link android.hardware.camera2.CameraDevice}.</p>
 *
 * @see android.media.Image
 * @see android.media.ImageReader
 * @see android.hardware.camera2.CameraDevice
 *
 * @hide
 */
#define Y16  0x20363159

/**
 * YCbCr format, used for video.
 *
 * <p>For the {@link android.hardware.camera2} API, the {@link #YUV_420_888} format is
 * recommended for YUV output instead.</p>
 *
 * <p>Whether this format is supported by the old camera API can be determined by
 * {@link android.hardware.Camera.Parameters#getSupportedPreviewFormats()}.</p>
 *
 */
#define NV16 0x10

/**
 * YCrCb format used for images, which uses the NV21 encoding format.
 *
 * <p>This is the default format
 * for {@link android.hardware.Camera} preview images, when not otherwise set with
 * {@link android.hardware.Camera.Parameters#setPreviewFormat(int)}.</p>
 *
 * <p>For the {@link android.hardware.camera2} API, the {@link #YUV_420_888} format is
 * recommended for YUV output instead.</p>
 */
#define NV21  0x11

/**
 * YCbCr format used for images, which uses YUYV (YUY2) encoding format.
 *
 * <p>For the {@link android.hardware.camera2} API, the {@link #YUV_420_888} format is
 * recommended for YUV output instead.</p>
 *
 * <p>This is an alternative format for {@link android.hardware.Camera} preview images. Whether
 * this format is supported by the camera hardware can be determined by
 * {@link android.hardware.Camera.Parameters#getSupportedPreviewFormats()}.</p>
 */
#define YUY2 0x14

/**
 * Compressed JPEG format.
 *
 * <p>This format is always supported as an output format for the
 * {@link android.hardware.camera2} API, and as a picture format for the older
 * {@link android.hardware.Camera} API</p>
 */
#define JPEG 0x100;

/**
 * <p>Multi-plane Android YUV 420 format</p>
 *
 * <p>This format is a generic YCbCr format, capable of describing any 4:2:0
 * chroma-subsampled planar or semiplanar buffer (but not fully interleaved),
 * with 8 bits per color sample.</p>
 *
 * <p>Images in this format are always represented by three separate buffers
 * of data, one for each color plane. Additional information always
 * accompanies the buffers, describing the row stride and the pixel stride
 * for each plane.</p>
 *
 * <p>The order of planes in the array returned by
 * {@link android.media.Image#getPlanes() Image#getPlanes()} is guaranteed such that
 * plane #0 is always Y, plane #1 is always U (Cb), and plane #2 is always V (Cr).</p>
 *
 * <p>The Y-plane is guaranteed not to be interleaved with the U/V planes
 * (in particular, pixel stride is always 1 in
 * {@link android.media.Image.Plane#getPixelStride() yPlane.getPixelStride()}).</p>
 *
 * <p>The U/V planes are guaranteed to have the same row stride and pixel stride
 * (in particular,
 * {@link android.media.Image.Plane#getRowStride() uPlane.getRowStride()}
 * == {@link android.media.Image.Plane#getRowStride() vPlane.getRowStride()} and
 * {@link android.media.Image.Plane#getPixelStride() uPlane.getPixelStride()}
 * == {@link android.media.Image.Plane#getPixelStride() vPlane.getPixelStride()};
 * ).</p>
 *
 * <p>For example, the {@link android.media.Image} object can provide data
 * in this format from a {@link android.hardware.camera2.CameraDevice}
 * through a {@link android.media.ImageReader} object.</p>
 *
 * @see android.media.Image
 * @see android.media.ImageReader
 * @see android.hardware.camera2.CameraDevice
 */
#define  YUV_420_888 0x23

/**
 * <p>Multi-plane Android YUV 422 format</p>
 *
 * <p>This format is a generic YCbCr format, capable of describing any 4:2:2
 * chroma-subsampled (planar, semiplanar or interleaved) format,
 * with 8 bits per color sample.</p>
 *
 * <p>Images in this format are always represented by three separate buffers
 * of data, one for each color plane. Additional information always
 * accompanies the buffers, describing the row stride and the pixel stride
 * for each plane.</p>
 *
 * <p>The order of planes in the array returned by
 * {@link android.media.Image#getPlanes() Image#getPlanes()} is guaranteed such that
 * plane #0 is always Y, plane #1 is always U (Cb), and plane #2 is always V (Cr).</p>
 *
 * <p>In contrast to the {@link #YUV_420_888} format, the Y-plane may have a pixel
 * stride greater than 1 in
 * {@link android.media.Image.Plane#getPixelStride() yPlane.getPixelStride()}.</p>
 *
 * <p>The U/V planes are guaranteed to have the same row stride and pixel stride
 * (in particular,
 * {@link android.media.Image.Plane#getRowStride() uPlane.getRowStride()}
 * == {@link android.media.Image.Plane#getRowStride() vPlane.getRowStride()} and
 * {@link android.media.Image.Plane#getPixelStride() uPlane.getPixelStride()}
 * == {@link android.media.Image.Plane#getPixelStride() vPlane.getPixelStride()};
 * ).</p>
 *
 * <p>For example, the {@link android.media.Image} object can provide data
 * in this format from a {@link android.media.MediaCodec}
 * through {@link android.media.MediaCodec#getOutputImage} object.</p>
 *
 * @see android.media.Image
 * @see android.media.MediaCodec
 */
#define YUV_422_888  0x27

/**
 * <p>Multi-plane Android YUV 444 format</p>
 *
 * <p>This format is a generic YCbCr format, capable of describing any 4:4:4
 * (planar, semiplanar or interleaved) format,
 * with 8 bits per color sample.</p>
 *
 * <p>Images in this format are always represented by three separate buffers
 * of data, one for each color plane. Additional information always
 * accompanies the buffers, describing the row stride and the pixel stride
 * for each plane.</p>
 *
 * <p>The order of planes in the array returned by
 * {@link android.media.Image#getPlanes() Image#getPlanes()} is guaranteed such that
 * plane #0 is always Y, plane #1 is always U (Cb), and plane #2 is always V (Cr).</p>
 *
 * <p>In contrast to the {@link #YUV_420_888} format, the Y-plane may have a pixel
 * stride greater than 1 in
 * {@link android.media.Image.Plane#getPixelStride() yPlane.getPixelStride()}.</p>
 *
 * <p>The U/V planes are guaranteed to have the same row stride and pixel stride
 * (in particular,
 * {@link android.media.Image.Plane#getRowStride() uPlane.getRowStride()}
 * == {@link android.media.Image.Plane#getRowStride() vPlane.getRowStride()} and
 * {@link android.media.Image.Plane#getPixelStride() uPlane.getPixelStride()}
 * == {@link android.media.Image.Plane#getPixelStride() vPlane.getPixelStride()};
 * ).</p>
 *
 * <p>For example, the {@link android.media.Image} object can provide data
 * in this format from a {@link android.media.MediaCodec}
 * through {@link android.media.MediaCodec#getOutputImage} object.</p>
 *
 * @see android.media.Image
 * @see android.media.MediaCodec
 */
#define YUV_444_888 0x28
#endif //FINENGINE_IMAGEFORMAT_H
