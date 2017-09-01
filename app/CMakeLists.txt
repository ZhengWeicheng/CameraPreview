# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
                native-lib

                # Sets the library as a shared library.
                SHARED

                # Provides a relative path to your source file(s).
                # Associated headers in the same location as their source
                # file are automatically included.
                #============utils=================
                src/main/cpp/utils/GLUtil.cpp
                src/main/cpp/utils/JNIUtils.cpp
                #============render=================
                src/main/cpp/render/DataRender.cpp
                src/main/cpp/render/TextureRender.cpp
                src/main/cpp/render/RecordCameraRender.cpp
                #============filter=================
                src/main/cpp/filter/BaseFilter.cpp
                src/main/cpp/filter/CameraTextureFilter.cpp
                src/main/cpp/filter/RecordFilter.cpp
                src/main/cpp/filter/RecordCameraFilter.cpp
                #==============encoder==================
                src/main/cpp/encode/jx_yuv_encode_h264.cpp
                src/main/cpp/encode/jx_pcm_encode_aac.cpp
                src/main/cpp/encode/jx_media_muxer.cpp
                src/main/cpp/encode/jx_jni_handler.cpp
                src/main/cpp/encode/jx_ffmpeg_jni.cpp
                src/main/cpp/encode/threadsafe_queue.cpp
                src/main/cpp/encode/jx_log.cpp
                src/main/cpp/encode/cmdutils.c
                src/main/cpp/encode/ffmpeg.c
                src/main/cpp/encode/ffmpeg_filter.c
                src/main/cpp/encode/ffmpeg_opt.c
                src/main/cpp/encode/jx_ffmpeg_cmd_run.c
                src/main/cpp/encode/jx_ffmpeg_config.cpp

                #src/main/cpp/h264encoder/X264Encoder.cpp
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

add_library(
            avcodec
            SHARED
            IMPORTED
            )

add_library(
            avfilter
            SHARED
            IMPORTED
             )


add_library(
            avformat
            SHARED
            IMPORTED
            )


add_library(
            avutil
            SHARED
            IMPORTED
            )

add_library(
            swresample
            SHARED
            IMPORTED
            )

add_library(
            swscale
            SHARED
            IMPORTED
            )

add_library(
            fdk-aac
            SHARED
            IMPORTED
            )


set_target_properties(
    avcodec
    PROPERTIES IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavcodec.so
    )

set_target_properties(
        avfilter
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavfilter.so
        )

set_target_properties(
            avformat
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavformat.so
            )

set_target_properties(
            avutil
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libavutil.so
            )

set_target_properties(
            swresample
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libswresample.so
             )

set_target_properties(
            swscale
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libswscale.so
             )

set_target_properties(
         fdk-aac
         PROPERTIES IMPORTED_LOCATION
          ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libfdk-aac.so
          )

include_directories(
 ${CMAKE_SOURCE_DIR}/src/main/cpp/encode/include/
 )

#===================================lib yuv =======================================
 add_library(
             libYUV
             SHARED
             IMPORTED
             )

set_target_properties(
         libYUV
         PROPERTIES IMPORTED_LOCATION
          ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libyuv.so
          )

include_directories(
${CMAKE_SOURCE_DIR}/src/main/cpp/libyuv/include/
)
#===================================lib yuv =======================================

#=================================== x264 =======================================
# add_library(
#            x264
#             SHARED
#             IMPORTED
#             )
#
#set_target_properties(
#         x264
#         PROPERTIES IMPORTED_LOCATION
#          ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libx264.so
#          )
#=================================== x264 =======================================

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib
                       EGL
                       #GLESv2
                       GLESv3
                       log
                       android
                       avcodec
                       avfilter
                       avformat
                       avutil
                       swresample
                       swscale
                       libYUV
                       #x264
                       )
