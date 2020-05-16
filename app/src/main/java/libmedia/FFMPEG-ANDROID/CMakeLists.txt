cmake_minimum_required(VERSION 3.4.1)

project(FFMPEG_ANDROID)

if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
    set(FFMPEG_ARCH_PREFIX "arm")
elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    set(FFMPEG_ARCH_PREFIX "arm64")
else()
    #     for x86 and x86_64 the CMAKE_ANDROID_ARCH_ABI variable is set to the same string
    set(FFMPEG_ARCH_PREFIX ${CMAKE_ANDROID_ARCH_ABI})
endif()

set(FFMPEG_PARENT_DIR ../../../../../../)
set(FFMPEG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${FFMPEG_PARENT_DIR}/ffmpeg)
set(PREBUILT ${FFMPEG_DIR}/prebuilt/android-${FFMPEG_ARCH_PREFIX})
set(PREBUILTESSENTIAL ${FFMPEG_DIR}/android/libs/${CMAKE_ANDROID_ARCH_ABI})

# we can take advantage of just including the prebuilt folder to have
# the IDE find the correct folders for each lib we want
# inctead of including each lib's folders manually

set(
        FFMPEG-ANDROID-INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/android-${FFMPEG_ARCH_PREFIX}
        PARENT_SCOPE
)

add_library(avcodec SHARED IMPORTED)
add_library(avdevice SHARED IMPORTED)
add_library(avfilter SHARED IMPORTED)
add_library(avformat SHARED IMPORTED)
add_library(avutil SHARED IMPORTED)
add_library(swresample SHARED IMPORTED)
add_library(swscale SHARED IMPORTED)
add_library(cpufeatures SHARED IMPORTED)
add_library(c++_shared SHARED IMPORTED)

# set_target_properties requires an absolute path, this path may contain ../

# it is currently unknown how to obtain an absolute path to the app's libs dir

# we assume a static location between FFMPEG-ANDROID and app/libs

set_target_properties(
        avcodec PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec.so
)

set_target_properties(
        avdevice PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libavdevice.so
)

set_target_properties(
        avfilter PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libavfilter.so
)

set_target_properties(
        avformat PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libavformat.so
)

set_target_properties(
        avutil PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libavutil.so
)

set_target_properties(
        swresample PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libswresample.so
)

set_target_properties(
        swscale PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libswscale.so
)

set_target_properties(
        cpufeatures PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libcpufeatures.so
)

set_target_properties(
        c++_shared PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libc++_shared.so
)

add_library(chromaprint STATIC IMPORTED)
set_target_properties(
        chromaprint PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/chromaprint/lib/libchromaprint.a
)

add_library(expat STATIC IMPORTED)
set_target_properties(
        expat PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/expat/lib/libexpat.a
)

add_library(fontconfig STATIC IMPORTED)
set_target_properties(
        fontconfig PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/fontconfig/lib/libfontconfig.a
)

add_library(freetype STATIC IMPORTED)
set_target_properties(
        freetype PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/freetype/lib/libfreetype.a
)

add_library(fribidi STATIC IMPORTED)
set_target_properties(
        fribidi PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/fribidi/lib/libfribidi.a
)

add_library(giflib STATIC IMPORTED)
set_target_properties(
        giflib PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/giflib/lib/libgif.a
)

add_library(gmp STATIC IMPORTED)
set_target_properties(
        gmp PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/gmp/lib/libgmp.a
)

add_library(gnutls STATIC IMPORTED)
set_target_properties(
        gnutls PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/gnutls/lib/libgnutls.a
)

add_library(gnutlsxx STATIC IMPORTED)
set_target_properties(
        gnutlsxx PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/gnutls/lib/libgnutlsxx.a
)

add_library(jpeg STATIC IMPORTED)
set_target_properties(
        jpeg PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/jpeg/lib/libjpeg.a
)

add_library(kvazaar STATIC IMPORTED)
set_target_properties(
        kvazaar PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/kvazaar/lib/libkvazaar.a
)

add_library(lame STATIC IMPORTED)
set_target_properties(
        lame PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/lame/lib/libmp3lame.a
)

add_library(leptonica STATIC IMPORTED)
set_target_properties(
        leptonica PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/leptonica/lib/liblept.a
)

add_library(libaom STATIC IMPORTED)
set_target_properties(
        libaom PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libaom/lib/libaom.a
)

add_library(libass STATIC IMPORTED)
set_target_properties(
        libass PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libass/lib/libass.a
)

add_library(libcharset STATIC IMPORTED)
set_target_properties(
        libcharset PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libiconv/lib/libcharset.a
)

add_library(libiconv STATIC IMPORTED)
set_target_properties(
        libiconv PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libiconv/lib/libiconv.a
)

add_library(libilbc STATIC IMPORTED)
set_target_properties(
        libilbc PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libilbc/lib/libilbc.a
)

add_library(libogg STATIC IMPORTED)
set_target_properties(
        libogg PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libogg/lib/libogg.a
)

add_library(libpng STATIC IMPORTED)
set_target_properties(
        libpng PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libpng/lib/libpng.a
)

add_library(libpng16 STATIC IMPORTED)
set_target_properties(
        libpng16 PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libpng/lib/libpng16.a
)

add_library(libsamplerate STATIC IMPORTED)
set_target_properties(
        libsamplerate PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libsamplerate/lib/libsamplerate.a
)

add_library(libsndfile STATIC IMPORTED)
set_target_properties(
        libsndfile PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libsndfile/lib/libsndfile.a
)

add_library(libtheora STATIC IMPORTED)
set_target_properties(
        libtheora PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libtheora/lib/libtheora.a
)

add_library(libtheoradec STATIC IMPORTED)
set_target_properties(
        libtheoradec PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libtheora/lib/libtheoradec.a
)

add_library(libtheoraenc STATIC IMPORTED)
set_target_properties(
        libtheoraenc PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libtheora/lib/libtheoraenc.a
)

add_library(libuuid STATIC IMPORTED)
set_target_properties(
        libuuid PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libuuid/lib/libuuid.a
)

add_library(libvidstab STATIC IMPORTED)
set_target_properties(
        libvidstab PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libvidstab/lib/libvidstab.a
)

add_library(libvorbis STATIC IMPORTED)
set_target_properties(
        libvorbis PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libvorbis/lib/libvorbis.a
)

add_library(libvorbisenc STATIC IMPORTED)
set_target_properties(
        libvorbisenc PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libvorbis/lib/libvorbisenc.a
)

add_library(libvorbisfile STATIC IMPORTED)
set_target_properties(
        libvorbisfile PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libvorbis/lib/libvorbisfile.a
)

add_library(libvpx STATIC IMPORTED)
set_target_properties(
        libvpx PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libvpx/lib/libvpx.a
)

add_library(libwebp STATIC IMPORTED)
set_target_properties(
        libwebp PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libwebp/lib/libwebp.a
)

add_library(libwebpdemux STATIC IMPORTED)
set_target_properties(
        libwebpdemux PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libwebp/lib/libwebpdemux.a
)

add_library(libwebpmux STATIC IMPORTED)
set_target_properties(
        libwebpmux PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libwebp/lib/libwebpmux.a
)

add_library(libxml2 STATIC IMPORTED)
set_target_properties(
        libxml2 PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/libxml2/lib/libxml2.a
)

add_library(nettle STATIC IMPORTED)
set_target_properties(
        nettle PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/nettle/lib/libnettle.a
)

add_library(libhogweed STATIC IMPORTED)
set_target_properties(
        libhogweed PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/nettle/lib/libhogweed.a
)

add_library(opencore-amrnb STATIC IMPORTED)
set_target_properties(
        opencore-amrnb PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/opencore-amr/lib/libopencore-amrnb.a
)

add_library(opencore-amrwb STATIC IMPORTED)
set_target_properties(
        opencore-amrwb PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/opencore-amr/lib/libopencore-amrwb.a
)

add_library(openh264 STATIC IMPORTED)
set_target_properties(
        openh264 PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/openh264/lib/libopenh264.a
)

add_library(opus STATIC IMPORTED)
set_target_properties(
        opus PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/opus/lib/libopus.a
)

add_library(rubberband STATIC IMPORTED)
set_target_properties(
        rubberband PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/rubberband/lib/librubberband.a
)

add_library(sdl STATIC IMPORTED)
set_target_properties(
        sdl PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/sdl/lib/libSDL2.a
)

add_library(sdltest STATIC IMPORTED)
set_target_properties(
        sdltest PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/sdl/lib/libSDL2_test.a
)

add_library(sdlmain STATIC IMPORTED)
set_target_properties(
        sdlmain PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/sdl/lib/libSDL2main.a
)

add_library(shine STATIC IMPORTED)
set_target_properties(
        shine PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/shine/lib/libshine.a
)

add_library(snappy STATIC IMPORTED)
set_target_properties(
        snappy PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/snappy/lib/libsnappy.a
)

add_library(soxr STATIC IMPORTED)
set_target_properties(
        soxr PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/soxr/lib/libsoxr.a
)

add_library(speex STATIC IMPORTED)
set_target_properties(
        speex PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/speex/lib/libspeex.a
)

add_library(tesseract STATIC IMPORTED)
set_target_properties(
        tesseract PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/tesseract/lib/libtesseract.a
)

add_library(tiff STATIC IMPORTED)
set_target_properties(
        tiff PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/tiff/lib/libtiff.a
)

add_library(twolame STATIC IMPORTED)
set_target_properties(
        twolame PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/twolame/lib/libtwolame.a
)

add_library(wavpack STATIC IMPORTED)
set_target_properties(
        wavpack PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/wavpack/lib/libwavpack.a
)

add_library(x264 STATIC IMPORTED)
set_target_properties(
        x264 PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/x264/lib/libx264.a
)

add_library(x265 STATIC IMPORTED)
set_target_properties(
        x265 PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/x265/lib/libx265.a
)

add_library(xvidcore STATIC IMPORTED)
set_target_properties(
        xvidcore PROPERTIES IMPORTED_LOCATION
        ${PREBUILT}/xvidcore/lib/libxvidcore.a
)

# empty.cpp is an empty C++ file in order to get libraries to compile

include_directories(include/android-${FFMPEG_ARCH_PREFIX})

add_library(ffmpeg SHARED
        cpp/fftools_cmdutils.c
        cpp/fftools_ffmpeg.c
        cpp/fftools_ffmpeg_filter.c
        cpp/fftools_ffmpeg_hw.c
        cpp/fftools_ffmpeg_opt.c
        cpp/fftools_ffprobe.c
        cpp/mobileffmpeg.c
        cpp/mobileffmpeg_abidetect.c
        cpp/mobileffmpeg_exception.c
        cpp/mobileffprobe.c
)

target_link_libraries(
        ffmpeg

        # shared libraries
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        cpufeatures
        c++_shared
        log
)

add_library (FFMPEG-ANDROID SHARED empty.cpp)

target_link_libraries (
        FFMPEG-ANDROID

        # shared libraries
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        cpufeatures
        c++_shared
        log

        # ffmpeg command line API shared library
        ffmpeg

        # static libraries
        chromaprint
        expat
        fontconfig
        freetype
        fribidi
        giflib
        gmp
        gnutls
        # omit gnutlsxx
        jpeg
        kvazaar
        lame
        leptonica
        libaom
        libass
        libcharset
        libiconv
        libilbc
        libogg
        libpng
        libpng16
        libsamplerate
        libsndfile
        libtheora
        libtheoradec
        libtheoraenc
        libuuid
        libvidstab
        libvorbis
        libvorbisenc
        libvorbisfile
        libvpx
        libwebp
        libwebpdemux
        libwebpmux
        libxml2
        nettle
        libhogweed
        opencore-amrnb
        opencore-amrwb
        openh264
        opus
        rubberband
        sdl
        sdltest
        sdlmain
        shine
        snappy
        soxr
        speex
        tesseract
        tiff
        twolame
        wavpack
        x264
        x265
        xvidcore
)