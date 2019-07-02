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
/*
#define MONITOR_TYPES std::variant<monitor<bool*>*,monitor<uint32_t*>*>

template <class TYPE> class monitor {
public:
    bool changed() {
        bool changed = false;
        for (unsigned int i = 0; i < a.size(); i++) {
            TYPE c1 = static_cast<TYPE>(a[i].current);
            TYPE c2 = static_cast<TYPE>(a[i].previous);
            if (*c1 != *c2) {
                *static_cast<TYPE>(a[i].previous) = *static_cast<TYPE>(a[i].current);
                LOGE("VALUE CHANGED");
                changed = true;
            }
        }
        return changed;
    }

    bool exists(std::string IDENTIFIER) {
        for (unsigned int i = 0; i < a.size(); i++)
            if (a[i].IDENTIFIER == IDENTIFIER)
                return true;
        return false;
    }

    int indexOf(std::string IDENTIFIER) {
        for (unsigned int i = 0; i < a.size(); i++)
            if (a[i].IDENTIFIER == IDENTIFIER)
                return i;
        return -1;
    }

    void add(std::string IDENTIFIER, TYPE what) {
        // could use exists but then it would loop twice
        int index = indexOf(IDENTIFIER);
        if (index != -1) {
            a[index].current = what;
        } else {
            LOGE("ADDING VALUE TO MONITOR LIST");
            mon m = mon();
            m.IDENTIFIER = IDENTIFIER;
            m.current = what;
            m.previous = static_cast<TYPE>(malloc(1*sizeof(TYPE)));
            a.push_front(m);
        }
    }

private:
    class mon {
    public:
        std::string IDENTIFIER = "";
        TYPE current = 0;
        TYPE previous = 0;
    };
    std::deque<mon> a = std::deque<mon>();
};

class monitorPool {
public:

    bool changed() {
        bool changed = false;
        for (unsigned int i = 0; i < a.size(); i++) {
            if (c(i)) {
                changed = true;
            }
        }
        return changed;
    }

    bool exists(std::string IDENTIFIER) {
        for (unsigned int i = 0; i < a.size(); i++)
            if (a[i].IDENTIFIER == IDENTIFIER)
                return true;
        return false;
    }

    int indexOf(std::string IDENTIFIER) {
        for (unsigned int i = 0; i < a.size(); i++)
            if (a[i].IDENTIFIER == IDENTIFIER)
                return i;
        return -1;
    }

    void add(std::string IDENTIFIER, MONITOR_TYPES what) {
        // could use exists but then it would loop twice
        int index = indexOf(IDENTIFIER);
        if (index != -1) {
            a[index].m = what;
        } else {
            LOGE("ADDING VALUE TO MONITOR LIST");
            mon m = mon();
            m.IDENTIFIER = IDENTIFIER;
            m.m = what;
            a.push_front(m);
        }
    }

    class mon {
    public:
        std::string IDENTIFIER = "";
        MONITOR_TYPES m;
    };

    std::deque<mon> a = std::deque<mon>();
    bool c(int index) {
        return std::visit([](MONITOR_TYPES &&arg) {
            if (std::holds_alternative<monitor<uint32_t*>*>(arg))
                return std::get<monitor<uint32_t *>*>(arg)->changed();
            else if (std::holds_alternative<monitor<bool*>*>(arg))
                return std::get<monitor<bool *>*>(arg)->changed();
        }, a[index].m);
    }
};
*/
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
} WVO;

static void fill_waveform(AndroidBitmapInfo *info, void *pixels, bool highlightSilence = false)
{
    bool USESAMPLE = false;
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"rendering sound form (Waveform)");
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"HEIGHT is %d", info->height);
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"WIDTH is %d", info->width);
    if (USESAMPLE) {
        int samples = info->width;
        samples = info->width;
        int16_t SCALED[samples];
        __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"generating test sample");
        int16_t outputData[samples];
        for (int i = 0; i < samples; ++i)
            outputData[i] = static_cast<int16_t>((info->height/2) * sin(i * (M_PI / 360)));
        __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"test sample generated");
        AudioTools::clone(outputData, SCALED, samples);
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"scaling width");
        int16_t TIMESTRETCHED[samples];
        AudioTools::zero(TIMESTRETCHED, samples);
        TimeStretch::Shorten::test(SCALED, samples, TIMESTRETCHED, info->width);
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"scaling height");
        AudioTools::scale(TIMESTRETCHED, TIMESTRETCHED, samples, static_cast<int16_t>(info->height));

        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"drawing");

        if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"BITMAP FORMAT RGBA_8888 UNSUPPORTED!");
            return;
        } else if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) {
            __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"BITMAP FORMAT RGB_565");
            for (int16_t column = 0; column < info->width; column++)
                pixelDraw_RGB_565(info, pixels, TIMESTRETCHED[column], column, pixelColourRGB_565(255, 255, 255));
        } else __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"BITMAP FORMAT UNKNOWN: %d", info->format);
    } else {
        uint64_t samples = WAVEFORMAUDIODATATOTALFRAMES;
        int16_t SCALED[samples];
        int16_t min = 0;
        int16_t max = 0;
        for (int i = 0; i < WAVEFORMAUDIODATATOTALFRAMES; ++i) {
            if (min > WAVEFORMAUDIODATA[i]) min = WAVEFORMAUDIODATA[i];
            if (max < WAVEFORMAUDIODATA[i]) max = WAVEFORMAUDIODATA[i];
        }
        __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"min = %d", min);
        __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"max = %d", max);
        AudioTools::clone(const_cast<int16_t *>(WAVEFORMAUDIODATA), SCALED, samples);
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"scaling width");
        int16_t TIMESTRETCHED[info->width]; AudioTools::zero(TIMESTRETCHED, info->width);
        if (WVO.stretchToScreenWidth) TimeStretch::Shorten::test(SCALED, samples, TIMESTRETCHED, info->width);
        else AudioTools::clone(SCALED, TIMESTRETCHED, info->width);
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"scaling height");
        if (WVO.stretchToScreenHeight) AudioTools::scale(TIMESTRETCHED, TIMESTRETCHED, info->width, static_cast<int16_t>(info->height));
        else {
            if (!WVO.stretchToScreenWidth) AudioTools::crop(SCALED, info->height, TIMESTRETCHED, info->width);
            else AudioTools::crop(TIMESTRETCHED, info->height, TIMESTRETCHED, info->width);
        }
//        int16_t TIMESTRETCHEDL[info->width]; AudioTools::zero(TIMESTRETCHEDL, info->width);
//        int16_t TIMESTRETCHEDR[info->width]; AudioTools::zero(TIMESTRETCHEDR, info->width);
//        AudioTools::splitStereo(TIMESTRETCHED, TIMESTRETCHEDL, TIMESTRETCHEDR, info->width);
//        AudioTools::clone(TIMESTRETCHEDL, TIMESTRETCHED, info->width);
        __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"drawing");

        Canvas canvas = Canvas(info, pixels);
        canvas.clear();
        canvas.buffered = false;
        canvas.color.set(255, 0, 0);
        if (info->format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"BITMAP FORMAT RGBA_8888 UNSUPPORTED!");
            return;
        } else if (info->format == ANDROID_BITMAP_FORMAT_RGB_565) {
            __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"BITMAP FORMAT RGB_565");
            for (int16_t column = 0; column < info->width; column++) {
                // highlight silence in green
                // silence threshold: positive 1 to negative 1
                if (WVO.highlightSilence) {
                    if (!(
                        (TIMESTRETCHED[column] - (info->height/2)) > 1 &&
                        (TIMESTRETCHED[column] - (info->height/2)) < -1)
                    ) {
                        canvas.color.save(canvas);
                        canvas.color.set(0, 255, 0);
                        if (WVO.drawLines)
                            canvas.line_segment(TIMESTRETCHED[column], column, TIMESTRETCHED[column + 1], column + 1);
                        else
                            canvas.pixel(TIMESTRETCHED[column], column);
                        canvas.color.restore(canvas);
                    }
                }
                else {
                    if (WVO.drawLines)
                        canvas.line_segment(TIMESTRETCHED[column], column, TIMESTRETCHED[column+1], column+1);
                    else
                        canvas.pixel(TIMESTRETCHED[column], column);
                }
            }
            canvas.flush();
        } else __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"BITMAP FORMAT UNKNOWN: %d", info->format);
    }

    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"rendered sound form (Waveform): %d frames rendered", pixelFrames);
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
    jclass c = env->FindClass("libmedia/Media$WaveformViewOptions__");
    while (!items.empty()) {
        javaClassToCppClassInformation info = items.front();
        if (!strcmp(info.type, "bool")) {
            *static_cast<bool*>(info.whereToPutParameter) =
                    env->GetBooleanField(clazz,env->GetFieldID(c,info.parameter,"Z"));
        }
        items.pop_front();
    }
}

extern "C" JNIEXPORT void JNICALL Java_libmedia_Media_00024internal_00024WaveformView_1_1_renderWaveform(JNIEnv * env, jobject  obj, jobject bitmap,  jlong  time_ms, jobject waveformOptionsClassInstance)
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
            fill_waveform(&info, pixels);
            AndroidBitmap_unlockPixels(env, bitmap);

            stats_endFrame(&stats, LOG_TAG);
        }
    }
}
