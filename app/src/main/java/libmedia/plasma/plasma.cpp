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

#define  LOG_TAG    "libplasma"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/* We're going to perform computations for every pixel of the target
 * bitmap. floating-point operations are very slow on ARMv5, and not
 * too bad on ARMv7 with the exception of trigonometric functions.
 *
 */

const int16_t *PLASMAAUDIODATA = nullptr;
uint64_t PLASMAAUDIODATATOTALFRAMES = 0;

constexpr int16_t Scale(int16_t data, int16_t height) // credits https://cpplang.slack.com/messages/@UFMQT6R8E Ashley Roll
{
    return static_cast<int16_t>(
            ((float)(data + 32768) / 65536.0F) * (float)height
    );
}

#include "pixelTypes.h"

static void fill_plasma(AndroidBitmapInfo *info, void *pixels)
{
    LOGI("rendering sound form (Waveform)");
    LOGI("HEIGHT is %d", info->height);
    LOGI("WIDTH is %d", info->width);
    int samples = info->width;
    int16_t min = 0;
    int16_t max = 0;
    for (int i = 0; i < samples; ++i) {
        if (min > PLASMAAUDIODATA[i]) min = PLASMAAUDIODATA[i];
        if (max < PLASMAAUDIODATA[i]) max = PLASMAAUDIODATA[i];
    }
    LOGE("min = %d", min);
    LOGE("max = %d", max);

    LOGI("scaling height");
    int16_t SCALED[samples];
    for (int i = 0; i<=samples; i++) SCALED[i] = Scale(PLASMAAUDIODATA[i], static_cast<int16_t>(info->height));
    LOGI("scaling width");

    LOGI("drawing");

    if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("BITMAP FORMAT RGBA_8888 UNSUPPORTED!");
        return;
        LOGI("BITMAP FORMAT RGBA_8888");
        for (int column = 0; column < samples; column++)
            drawPixel<uint32_t>(pixels, SCALED[column], column, 0xFFFFFFFFu, info->stride);
    } else if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGI("BITMAP FORMAT RGB_565");
        for (int column = 0; column < samples; column++)
            pixelDraw_RGB_565(info, pixels, SCALED[column], column, pixelColourRGB_565(255, 255, 255));
    } else LOGE("BITMAP FORMAT UNKNOWN: %d", info->format);
    LOGI("rendered sound form (Waveform)");
}

#include "timestats.h"

extern "C" JNIEXPORT void JNICALL Java_media_player_pro_PlasmaView_renderPlasma(JNIEnv * env, jobject  obj, jobject bitmap,  jlong  time_ms)
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

    if (PLASMAAUDIODATA != nullptr) {
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

            /* Now fill the values with a nice little plasma */
            fill_plasma(&info, pixels);

            AndroidBitmap_unlockPixels(env, bitmap);

            stats_endFrame(&stats);
        }
    }
}

//#include "plasmaARBG8888.h"