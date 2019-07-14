/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <cmath>
#include <stdlib.h>
#include "../AndroidBitmap/pixelTypes.h"
#include <cstring>
#include <cstdint>
#include <string>
#include <variant>
#include <any>
#include "TimeStretch.h"
#include "AudioTools.h"
#include "timestats.h"
#include "../AndroidBitmap/bitmap.h"
#include <MonitorPool.h>

MonitorPool MOMO;
Monitor<uint32_t*> MONITOR_uint32_t;
Monitor<bool*> MONITOR_bool;

#define  LOG_TAG    "libwaveform"

/* We're going to perform computations for every pixel of the target
 * bitmap. floating-point operations are very slow on ARMv5, and not
 * too bad on ARMv7 with the exception of trigonometric functions.
 *
 */

const int16_t *WAVEFORMAUDIODATA = nullptr;
uint64_t WAVEFORMAUDIODATATOTALFRAMES = 0;

class WaveformViewOptions {
public:
    bool drawLines = false;
    bool highlightSilence = false;
    bool stretchToScreenHeight = true;
    bool stretchToScreenWidth = false;
    // used internally to differentiate between stereo and mono
    bool LEFT = true;
    bool STEREO = true;
} WVO;

static void fill_waveform_stereo(AndroidBitmapInfo *infoLeft, AndroidBitmapInfo *infoRight, void *pixelsLeft, void *pixelsRight, bool highlightSilence = false) {
    uint32_t LH = 0;
    uint32_t LW = 0;
    uint32_t RH = 0;
    uint32_t RW = 0;
    if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO) {
            LH = infoLeft->height;
            LW = infoLeft->width;
    }
    if (WVO.STEREO && !WVO.LEFT) {
        RH = infoRight->height;
        RW = infoRight->width;
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "rendering sound form (Waveform)");
    if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "HEIGHT is %d", LH);
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "WIDTH is %d", LW);
    }
    if (WVO.STEREO && !WVO.LEFT) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "HEIGHT is %d", RH);
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "WIDTH is %d", RW);
    }
    uint64_t samples = WAVEFORMAUDIODATATOTALFRAMES;
    if (WVO.STEREO)
        samples /= 2;
    int16_t L[samples];
    int16_t R[samples];
    int16_t min = 0;
    int16_t max = 0;
    for (int i = 0; i < WAVEFORMAUDIODATATOTALFRAMES; ++i) {
        if (min > WAVEFORMAUDIODATA[i]) min = WAVEFORMAUDIODATA[i];
        if (max < WAVEFORMAUDIODATA[i]) max = WAVEFORMAUDIODATA[i];
    }
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "min = %d", min);
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "max = %d", max);
    if (WVO.STEREO)
        AudioTools::splitStereo(const_cast<int16_t *>(WAVEFORMAUDIODATA), L, R, samples * 2);
    else
        AudioTools::clone(const_cast<int16_t *>(WAVEFORMAUDIODATA), L, samples);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "scaling width");
    int16_t TIMESTRETCHEDL[LW];
    AudioTools::zero(TIMESTRETCHEDL, LW);
    int16_t TIMESTRETCHEDR[RW];
    AudioTools::zero(TIMESTRETCHEDR, RW);
    if (WVO.stretchToScreenWidth) {
        if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
            TimeStretch::Shorten::test(L, samples, TIMESTRETCHEDL, LW);
        if (WVO.STEREO && !WVO.LEFT)
                TimeStretch::Shorten::test(R, samples, TIMESTRETCHEDR, RW);
    } else {
        if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
            AudioTools::clone(L, TIMESTRETCHEDL, LW);
        if (WVO.STEREO && !WVO.LEFT)
            AudioTools::clone(R, TIMESTRETCHEDR, RW);
    }
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "scaling height");
    if (WVO.stretchToScreenHeight) {
        if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
            AudioTools::scale(TIMESTRETCHEDL, TIMESTRETCHEDL, LW, static_cast<int16_t>(LH));
        if (WVO.STEREO && !WVO.LEFT)
            AudioTools::scale(TIMESTRETCHEDR, TIMESTRETCHEDR, RW, static_cast<int16_t>(RH));
    } else {
        if (!WVO.stretchToScreenWidth) {
            if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
                AudioTools::crop(L, LH, TIMESTRETCHEDL, LW);
            if (WVO.STEREO && !WVO.LEFT)
                AudioTools::crop(R, RH, TIMESTRETCHEDR, RW);
        } else {
            if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
                AudioTools::crop(TIMESTRETCHEDL, LH, TIMESTRETCHEDL, LW);
            if (WVO.STEREO && !WVO.LEFT)
                AudioTools::crop(TIMESTRETCHEDR, RH, TIMESTRETCHEDR, RW);
        }
    }
//        AudioTools::splitStereo(TIMESTRETCHED, TIMESTRETCHEDL, TIMESTRETCHEDR, LW);
//        AudioTools::clone(TIMESTRETCHEDL, TIMESTRETCHED, LW);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "drawing");

    Canvas canvasLeft = Canvas(infoLeft, pixelsLeft);
    Canvas canvasRight = Canvas(infoRight, pixelsRight);
    if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
        canvasLeft.clear();
    if (WVO.STEREO && !WVO.LEFT)
        canvasRight.clear();
    if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
        canvasLeft.buffered = false;
    if (WVO.STEREO && !WVO.LEFT)
        canvasRight.buffered = false;
    if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
        canvasLeft.color.set(255, 0, 0);
    if (WVO.STEREO && !WVO.LEFT)
        canvasRight.color.set(255, 0, 0);
    bool formatsupported = false;
    if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
        if (infoLeft->format == ANDROID_BITMAP_FORMAT_RGB_565)
            formatsupported = true;
    if (WVO.STEREO && !WVO.LEFT)
        if (infoRight->format != ANDROID_BITMAP_FORMAT_RGB_565)
            formatsupported = false;
    if (formatsupported) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "BITMAP FORMAT RGB_565");
        if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO) {
            for (int16_t column = 0; column < LW; column++) {
                // highlight silence in green
                // silence threshold: positive 1 to negative 1
                if (WVO.highlightSilence) {
                    if (!(
                            (TIMESTRETCHEDL[column] - (LH / 2)) > 1 &&
                            (TIMESTRETCHEDL[column] - (LH / 2)) < -1)
                            ) {
                        canvasLeft.color.save(canvasLeft);
                        canvasLeft.color.set(0, 255, 0);
                        if (WVO.drawLines)
                            canvasLeft.line_segment(TIMESTRETCHEDL[column], column, TIMESTRETCHEDL[column + 1],
                                                    column + 1);
                        else
                            canvasLeft.pixel(TIMESTRETCHEDL[column], column);
                        canvasLeft.color.restore(canvasLeft);
                    }
                } else {
                    if (WVO.drawLines)
                        canvasLeft.line_segment(TIMESTRETCHEDL[column], column, TIMESTRETCHEDL[column + 1], column + 1);
                    else
                        canvasLeft.pixel(TIMESTRETCHEDL[column], column);
                }
            }
            canvasLeft.flush();
        }
        if (WVO.STEREO && !WVO.LEFT) {
            for (int16_t column = 0; column < RW; column++) {
                // highlight silence in green
                // silence threshold: positive 1 to negative 1
                if (WVO.highlightSilence) {
                    if (!(
                            (TIMESTRETCHEDR[column] - (LH / 2)) > 1 &&
                            (TIMESTRETCHEDR[column] - (LH / 2)) < -1)
                            ) {
                        canvasRight.color.save(canvasRight);
                        canvasRight.color.set(0, 255, 0);
                        if (WVO.drawLines)
                            canvasRight.line_segment(TIMESTRETCHEDR[column], column, TIMESTRETCHEDR[column + 1],
                                                     column + 1);
                        else
                            canvasRight.pixel(TIMESTRETCHEDR[column], column);
                        canvasRight.color.restore(canvasRight);
                    }
                } else {
                    if (WVO.drawLines)
                        canvasRight.line_segment(TIMESTRETCHEDR[column], column, TIMESTRETCHEDR[column + 1],
                                                 column + 1);
                    else
                        canvasRight.pixel(TIMESTRETCHEDR[column], column);
                }
            }
            canvasRight.flush();
        }
    } else {
        if ((WVO.STEREO && WVO.LEFT) || !WVO.STEREO)
            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"BITMAP FORMAT UNSUPPORTED: %d", infoLeft->format);
        if (WVO.STEREO && !WVO.LEFT)
            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"BITMAP FORMAT UNSUPPORTED: %d", infoRight->format);
    }
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"rendered sound form (Waveform)");
}

static void fill_waveform_mono(AndroidBitmapInfo *info, void *pixels) {
    WVO.STEREO = false;
    WVO.LEFT = true;
    fill_waveform_stereo(info, nullptr, pixels, nullptr);
}


class javaClassToCppClassInformation {
public:
    javaClassToCppClassInformation(const char *type, const char *parameter, void *whereToPutParameter)
            : parameter(parameter), type(type), whereToPutParameter(whereToPutParameter) {}

public:
    const char * type;
    const char * parameter;
    void * whereToPutParameter;
};

void javaClassToCppClass(JNIEnv * env, jobject clazz, std::deque<javaClassToCppClassInformation> items) {
    // internal classes are accessed via $
    jclass c = env->FindClass("libmedia/Media$Classes$WaveformViewOptions__");
    while (!items.empty()) {
        javaClassToCppClassInformation info = items.front();
        if (!strcmp(info.type, "bool")) {
            *static_cast<bool*>(info.whereToPutParameter) =
                    env->GetBooleanField(clazz,env->GetFieldID(c,info.parameter,"Z"));
        }
        items.pop_front();
    }
}

extern "C" JNIEXPORT void JNICALL Java_libmedia_Media_00024Classes_00024WaveformView_1_1_renderWaveformMono(JNIEnv * env, jobject  obj, jobject bitmap, jlong  time_ms, jobject waveformOptionsClassInstance)
{
    std::deque<javaClassToCppClassInformation> a = std::deque<javaClassToCppClassInformation>();
    a.push_front(javaClassToCppClassInformation("bool", "drawLines", &WVO.drawLines));
    a.push_front(javaClassToCppClassInformation("bool", "highlightSilence", &WVO.highlightSilence));
    a.push_front(javaClassToCppClassInformation("bool", "stretchToScreenWidth", &WVO.stretchToScreenWidth));
    a.push_front(javaClassToCppClassInformation("bool", "stretchToScreenHeight", &WVO.stretchToScreenHeight));
    javaClassToCppClass(env, waveformOptionsClassInstance, a);

    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;
    static bool         init = false;
    if (!init) { // draw once
        stats_init(&stats);
        init = true;
        MOMO.add("MONITOR_uint32_t", &MONITOR_uint32_t);
        MOMO.add("MONITOR_bool", &MONITOR_bool);
    }

    if (WAVEFORMAUDIODATA != nullptr) {
        if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"AndroidBitmap_getInfo() failed ! error=%d", ret);
            return;
        }
        MONITOR_uint32_t.add("INFO HEIGHT", &info.height);
        MONITOR_uint32_t.add("INFO WIDTH", &info.width);
        MONITOR_bool.add("LINES", &WVO.drawLines);
        MONITOR_bool.add("HIGHLIGHT", &WVO.highlightSilence);
        MONITOR_bool.add("stretch screen height", &WVO.stretchToScreenHeight);
        MONITOR_bool.add("stretch screen width", &WVO.stretchToScreenWidth);
        if (MOMO.changed()) {

            if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
                __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"AndroidBitmap_lockPixels() failed ! error=%d", ret);
            }

            stats_startFrame(&stats);

            /* Now fill the values with a nice little waveform */
            fill_waveform_mono(&info, pixels);
            AndroidBitmap_unlockPixels(env, bitmap);

            stats_endFrame(&stats, LOG_TAG);
        }
    }
}

extern "C" JNIEXPORT void JNICALL Java_libmedia_Media_00024Classes_00024WaveformView_1_1_renderWaveformStereo(JNIEnv * env, jobject  obj, jobject bitmapLeft, jobject bitmapRight, jlong  time_ms, jobject waveformOptionsClassInstance)
{
    std::deque<javaClassToCppClassInformation> a = std::deque<javaClassToCppClassInformation>();
    a.push_front(javaClassToCppClassInformation("bool", "drawLines", &WVO.drawLines));
    a.push_front(javaClassToCppClassInformation("bool", "highlightSilence", &WVO.highlightSilence));
    a.push_front(javaClassToCppClassInformation("bool", "stretchToScreenWidth", &WVO.stretchToScreenWidth));
    a.push_front(javaClassToCppClassInformation("bool", "stretchToScreenHeight", &WVO.stretchToScreenHeight));
    javaClassToCppClass(env, waveformOptionsClassInstance, a);

    AndroidBitmapInfo  infoLeft;
//    AndroidBitmapInfo  infoRight;
    void*              pixelsLeft;
//    void*              pixelsRight;
    int                retLeft;
//    int                retRight;
    static Stats       statsLeft;
//    static Stats       statsRight;
    static bool         init = false;
    if (!init) { // draw once
        stats_init(&statsLeft);
//        stats_init(&statsRight);
        init = true;
        MOMO.add("MONITOR_uint32_t", &MONITOR_uint32_t);
        MOMO.add("MONITOR_bool", &MONITOR_bool);
    }

    if (WAVEFORMAUDIODATA != nullptr) {
        if ((retLeft = AndroidBitmap_getInfo(env, bitmapLeft, &infoLeft)) < 0) {
            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"AndroidBitmap_getInfo() failed ! error=%d", retLeft);
            return;
        }
//        if ((retRight = AndroidBitmap_getInfo(env, bitmapRight, &infoRight)) < 0) {
//            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"AndroidBitmap_getInfo() failed ! error=%d", retRight);
//            return;
//        }
        MONITOR_uint32_t.add("INFO HEIGHT", &infoLeft.height);
        MONITOR_uint32_t.add("INFO WIDTH", &infoLeft.width);
//        MONITOR_uint32_t.add("INFO HEIGHT", &infoRight.height);
//        MONITOR_uint32_t.add("INFO WIDTH", &infoRight.width);
        MONITOR_bool.add("LINES", &WVO.drawLines);
        MONITOR_bool.add("HIGHLIGHT", &WVO.highlightSilence);
        MONITOR_bool.add("stretch screen height", &WVO.stretchToScreenHeight);
        MONITOR_bool.add("stretch screen width", &WVO.stretchToScreenWidth);
        if (MOMO.changed()) {

            if ((retLeft = AndroidBitmap_lockPixels(env, bitmapLeft, &pixelsLeft)) < 0) {
                __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"AndroidBitmap_lockPixels() failed ! error=%d", retLeft);
            }

//            if ((retRight = AndroidBitmap_lockPixels(env, bitmapRight, &pixelsRight)) < 0) {
//                __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"AndroidBitmap_lockPixels() failed ! error=%d", retRight);
//            }

            WVO.LEFT = true;
            stats_startFrame(&statsLeft);

            /* Now fill the values with a nice little waveform */
            fill_waveform_stereo(&infoLeft, nullptr, pixelsLeft, nullptr);
            AndroidBitmap_unlockPixels(env, bitmapLeft);

            stats_endFrame(&statsLeft, LOG_TAG);

//            WVO.LEFT = false;
//            stats_startFrame(&statsRight);

            /* Now fill the values with a nice little waveform */
//            fill_waveform_stereo(nullptr, &infoRight, nullptr, pixelsRight);
//            AndroidBitmap_unlockPixels(env, bitmapRight);

//            stats_endFrame(&statsLeft, LOG_TAG);
        }
    }
}
