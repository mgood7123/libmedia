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

#define  LOG_TAG    "libwaveform"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/* We're going to perform computations for every pixel of the target
 * bitmap. floating-point operations are very slow on ARMv5, and not
 * too bad on ARMv7 with the exception of trigonometric functions.
 *
 */

const int16_t *WAVEFORMAUDIODATA = nullptr;
uint64_t WAVEFORMAUDIODATATOTALFRAMES = 0;

#include "pixelTypes.h"
#include <cstring>
#include "TimeStretch.h"
#include "AudioTools.h"

static void fill_waveform(AndroidBitmapInfo *info, void *pixels)
{
    bool USESAMPLE = false;
    LOGI("rendering sound form (Waveform)");
    LOGI("HEIGHT is %d", info->height);
    LOGI("WIDTH is %d", info->width);
    if (USESAMPLE) {
        int samples = info->width;
        samples = info->width;
        int16_t SCALED[samples];
        LOGE("generating test sample");
        int16_t outputData[samples];
        for (int i = 0; i < samples; ++i)
            outputData[i] = static_cast<int16_t>((info->height/2) * sin(i * (M_PI / 360)));
        LOGE("test sample generated");
        AudioTools::clone(outputData, SCALED, samples);
        LOGI("scaling width");
        int16_t TIMESTRETCHED[samples];
        AudioTools::zero(TIMESTRETCHED, samples);
        TimeStretch::Shorten::test(SCALED, samples, TIMESTRETCHED, info->width);
        LOGI("scaling height");
        AudioTools::scale(TIMESTRETCHED, TIMESTRETCHED, samples, static_cast<int16_t>(info->height));

        LOGI("drawing");

        if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGE("BITMAP FORMAT RGBA_8888 UNSUPPORTED!");
            return;
            LOGI("BITMAP FORMAT RGBA_8888");
            for (int16_t column = 0; column < info->width; column++)
                pixelDraw<uint32_t>(pixels, SCALED[column], column, 0xFFFFFFFFu, info->stride);
        } else if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) {
            LOGI("BITMAP FORMAT RGB_565");
            for (int16_t column = 0; column < info->width; column++)
                pixelDraw_RGB_565(info, pixels, TIMESTRETCHED[column], column, pixelColourRGB_565(255, 255, 255));
        } else LOGE("BITMAP FORMAT UNKNOWN: %d", info->format);
    } else {
        uint64_t samples = WAVEFORMAUDIODATATOTALFRAMES;
        int16_t SCALED[samples];
        int16_t min = 0;
        int16_t max = 0;
        for (int i = 0; i < WAVEFORMAUDIODATATOTALFRAMES; ++i) {
            if (min > WAVEFORMAUDIODATA[i]) min = WAVEFORMAUDIODATA[i];
            if (max < WAVEFORMAUDIODATA[i]) max = WAVEFORMAUDIODATA[i];
        }
        LOGE("min = %d", min);
        LOGE("max = %d", max);
        AudioTools::clone(const_cast<int16_t *>(WAVEFORMAUDIODATA), SCALED, samples);
        LOGI("scaling width");
        int16_t TIMESTRETCHED[info->width];
        AudioTools::zero(TIMESTRETCHED, info->width);
        TimeStretch::Shorten::test(SCALED, samples, TIMESTRETCHED, info->width);
        LOGI("scaling height");
        AudioTools::scale(TIMESTRETCHED, TIMESTRETCHED, info->width, static_cast<int16_t>(info->height));

        LOGI("drawing");

        if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGE("BITMAP FORMAT RGBA_8888 UNSUPPORTED!");
            return;
            LOGI("BITMAP FORMAT RGBA_8888");
            for (int16_t column = 0; column < info->width; column++)
                pixelDraw<uint32_t>(pixels, TIMESTRETCHED[column], column, 0xFFFFFFFFu, info->stride);
        } else if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) {
            LOGI("BITMAP FORMAT RGB_565");
            for (int16_t column = 0; column < info->width; column++)
                pixelDraw_RGB_565(info, pixels, TIMESTRETCHED[column], column, pixelColourRGB_565(255, 255, 255));
        } else LOGE("BITMAP FORMAT UNKNOWN: %d", info->format);
    }

    LOGI("rendered sound form (Waveform): %d frames rendered", pixelFrames);
}

#include "timestats.h"

extern "C" JNIEXPORT void JNICALL Java_media_player_pro_WaveformView_renderWaveform(JNIEnv * env, jobject  obj, jobject bitmap,  jlong  time_ms)
{
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;
    static Stats       stats;
    static int         init;
    if (!init) { // draw once
        stats_init(&stats);
        init = 1;
    }

    static uint32_t height = 0;
    static uint32_t width = 0;
    static uint32_t heightP = 0;
    static uint32_t widthP = 0;

    if (WAVEFORMAUDIODATA != nullptr) {
        if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
            LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
            return;
        }
        height = info.height;
        width = info.height;
        if (height != heightP || width != widthP) {
            heightP = height;
            widthP = width;

            if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
                LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            }

            stats_startFrame(&stats);

            /* Now fill the values with a nice little waveform */
            fill_waveform(&info, pixels);

            AndroidBitmap_unlockPixels(env, bitmap);

            stats_endFrame(&stats);
        }
    }
}
