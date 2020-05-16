//
// Created by macropreprocessor on 18/06/19.
//

#ifndef MEDIA_PLAYER_PRO_AUDIOTIME_H
#define MEDIA_PLAYER_PRO_AUDIOTIME_H

#include <string>
#include <sstream>
#include <iostream>
#include <chrono>
#include <cmath>
#include <jni.h>
#include <android/bitmap.h>
#include "../waveform/timestats.h"
#include <date/date.h>
#include "../shell/regex_str.h"
using AudioTime_Format = uint64_t;
// declare after AudioTime_Format due to ifdef guard
#include "../AudioEngine/audio/SoundRecordingAudioData.h"
#include "timer.h"

#define ATM(include_human_time_markings, what, pointer, time, len) \
    if (time == time##Previous) { \
        /* increase the pointer, do nothing else */ \
        pointer+=(len); \
    } else if (time == 0) { \
        for (int i = 0; i < len; i++) (what+pointer)[i] = '0'; \
        pointer+=(len); \
    } else { \
        unsigned int x = 0; \
        for(unsigned int i = len; i != 0; i--) { \
            unsigned int n = i-1; \
            AudioTime_Format k = time; \
            while(n--) k/=10; \
            (what+pointer)[x++] = '0' + static_cast<int8_t>(k%10) /* this will be a single digit */; \
        } \
        pointer+=(len); \
    }

#define ATMSCA(include_human_time_markings, time, len) \
    if (time == time##_CACHE) { \
        /* do nothing */ \
    } else if (time == 0) { \
        memset(time##Str, '0', len); \
        time##_CACHE = time; \
    } else { \
        unsigned __int128 t = 1; \
        for(int i = 1; i < 129; i++) { \
            t *= 10; \
            if (time < t) { /* if t is 10, time has to be 0 to 9 */ \
                \
                /* this takes 6 microseconds */ \
                /* for (int i = 0; i != len */ /* if len is 4 then 0, 1, 2, 3, break at 4 */ /*; i++) (time##Str)[i] = '0'; */ \
                \
                /* this takes 35 microseconds */ \
                unsigned int x = 0; \
                for(unsigned int ii = i ; ii != 0 ; ii--) { \
                    unsigned int n = ii-1; /* n is 0 */ \
                    AudioTime_Format k = time; /* k is 10 */ \
                    while(n--) k/=10; /* while n != 0 */ \
                    (time##Str)[x++] = '0' + static_cast<int8_t>(k%10) /* this will be a single digit */; \
                } \
                break; \
            } \
        } \
        time##_CACHE = time; \
    }

#define ATMSCB(include_human_time_markings, time, len) \
    if (time == time##_CACHE) { \
        /* do nothing */ \
    } else if (time == 0) { \
        memset(time##Str, '0', len); \
        time##_CACHE = time; \
    } else { \
        uint64_t UPPER = 0, LOWER = 0; \
        std::string u, l; \
        const char * uu = nullptr, * ll = nullptr; \
        size_t ls = 0, us = 0; \
        UPPER = static_cast<uint64_t>(time >> 64); \
        LOWER = static_cast<uint64_t>(time); \
        if (UPPER) u = std::to_string(UPPER); \
        uu = u.c_str(); \
        l = std::to_string(LOWER); \
        ll = l.c_str(); \
        if (UPPER) us = strlen(uu); \
        ls = strlen(ll); \
        if (UPPER) memcpy(time##Str, uu, us); \
        memcpy(time##Str+us, ll, ls); \
        time##Str[us+ls+1] = '\0'; \
        time##_CACHE = time; \
    }

static char *qtoa(__uint128_t n) {
    static char buf[40];
    unsigned int i, j, m = 39;
    memset(buf, 0, 40);
    for (i = 128; i-- > 0;) {
        int carry = !!(n & (static_cast<__uint128_t>(1) << i));
        for (j = 39; j-- > m + 1 || carry;) {
            int d = 2 * buf[j] + carry;
            carry = d > 9;
            buf[j] = carry ? d - 10 : d;
        }
        m = j;
    }
    for (i = 0; i < 38; i++) {
        if (buf[i]) {
            break;
        }
    }
    for (j = i; j < 39; j++) {
        buf[j] += '0';
    }
    return buf + i;
}

#define ATMSCC(include_human_time_markings, time, len) \
    if (time == time##_CACHE) { \
        /* do nothing */ \
    } else if (time == 0) { \
        memset(time##Str, '0', len); \
        time##_CACHE = time; \
    } else { \
        memcpy(time##Str, qtoa(time), 40); \
        time##_CACHE = time; \
    }

#define ATMSC ATMSCB

class AudioTime {
public:
    class duration {
    public:
        typedef std::ratio<1l, 1000000000l> nano;
        typedef std::chrono::duration<unsigned long long,         std::nano> nanoseconds;
        typedef std::chrono::duration<unsigned long long,        std::micro> microseconds;
        typedef std::chrono::duration<unsigned long long,        std::milli> milliseconds;
        typedef std::chrono::duration<unsigned long long                   > seconds;
        typedef std::chrono::duration<     unsigned long, std::ratio<  60> > minutes;
        typedef std::chrono::duration<     unsigned long, std::ratio<3600> > hours;
        typedef date::days days;
        typedef date::weeks weeks;
        typedef date::months months;
        typedef date::years years;
    };

    AudioTime_Format nanosecondsTotal = 0;
    AudioTime_Format nanoseconds = 0;
    AudioTime_Format nanosecondsPrevious = 0;
    AudioTime_Format microsecondsTotal = 0;
    AudioTime_Format microseconds = 0;
    AudioTime_Format microsecondsPrevious = 0;
    AudioTime_Format millisecondsTotal = 0;
    AudioTime_Format milliseconds = 0;
    AudioTime_Format millisecondsPrevious = 0;
    AudioTime_Format secondsTotal = 0;
    AudioTime_Format seconds = 0;
    AudioTime_Format secondsPrevious = 0;
    AudioTime_Format minutesTotal = 0;
    AudioTime_Format minutes = 0;
    AudioTime_Format minutesPrevious = 0;
    AudioTime_Format hoursTotal = 0;
    AudioTime_Format hours = 0;
    AudioTime_Format hoursPrevious = 0;
    AudioTime_Format daysTotal = 0;
    AudioTime_Format days = 0;
    AudioTime_Format daysPrevious = 0;
    AudioTime_Format weeksTotal = 0;
    AudioTime_Format weeks = 0;
    AudioTime_Format weeksPrevious = 0;
    AudioTime_Format monthsTotal = 0;
    AudioTime_Format months = 0;
    AudioTime_Format monthsPrevious = 0;
    AudioTime_Format yearsTotal = 0;
    AudioTime_Format years = 0;
    AudioTime_Format yearsPrevious = 0;
    uint64_t currentFrame = 0;
    uint64_t previousFrame = 0;
    uint64_t totalFrames = 0;

    AudioTime_Format cb = 0;
    bool StartOfFile = false;
    bool EndOfFile = false;
    bool EndOfFileCalled = false;
    bool initializing = false;
    bool executeCallbacks = true;
    bool includeTimingInformation = true;

    class divisionValue {
    public:
        AudioTime_Format nanoseconds = 1000000000;
        AudioTime_Format microseconds = 1000000;
        AudioTime_Format milliseconds = 1000;
        AudioTime_Format seconds = 1;
        AudioTime_Format minutes = 60;
        AudioTime_Format hours = 3600;
        AudioTime_Format days = 3600; // TODO
        AudioTime_Format weeks = 3600; // TODO
        AudioTime_Format months = 3600; // TODO
        AudioTime_Format years = 3600; // TODO
    };

    class types {
    public:
        int nanoseconds = 1;
        int microseconds = 2;
        int milliseconds = 3;
        int seconds = 4;
        int minutes = 5;
        int hours = 6;
        int days = 7;
        int weeks = 8;
        int months = 9;
        int years = 10;
    };

    void update(uint64_t frame, SoundRecordingAudioData *AudioData);
    char * format(bool include_human_time_markings, SoundRecordingAudioData * AudioData);
    char * format(bool include_human_time_markings);
    void AudioTimeFormat(bool include_human_time_markings, char ** string, AudioTime_Format type);
    void AudioTimeFormat(bool include_human_time_markings, char ** string, AudioTime_Format type, int minimum_length);
    uint64_t toFrame(AudioTime_Format value, int type, SoundRecordingAudioData *AudioData);

    /**
     * provided callbacks:
     *
     * void StartOfFile(AudioTime * currentTime)
     *
     * void EndOfFile(AudioTime * currentTime)
     *
     * void nanosecondHundreth(AudioTime * currentTime)
     *
     * void nanosecondTenth(AudioTime * currentTime)
     *
     * void nanosecond(AudioTime * currentTime)
     *
     * void microsecondHundreth(AudioTime * currentTime)
     *
     * void microsecondTenth(AudioTime * currentTime)
     *
     * void microsecond(AudioTime * currentTime)
     *
     * void millisecondHundreth(AudioTime * currentTime)
     *
     * void millisecondTenth(AudioTime * currentTime)
     *
     * void millisecond(AudioTime * currentTime)
     *
     * void secondHundreth(AudioTime * currentTime)
     *
     * void secondTenth(AudioTime * currentTime)
     *
     * void second(AudioTime * currentTime)
     *
     * void minuteHundreth(AudioTime * currentTime)
     *
     * void minuteTenth(AudioTime * currentTime)
     *
     * void minute(AudioTime * currentTime)
     *
     * void hourHundreth(AudioTime * currentTime)
     *
     * void hourTenth(AudioTime * currentTime)
     *
     * void hour(AudioTime * currentTime)
     *
     * void dayHundreth(AudioTime * currentTime)
     *
     * void dayTenth(AudioTime * currentTime)
     *
     * void day(AudioTime * currentTime)
     *
     * void weekHundreth(AudioTime * currentTime)
     *
     * void weekTenth(AudioTime * currentTime)
     *
     * void week(AudioTime * currentTime)
     *
     * void monthHundreth(AudioTime * currentTime)
     *
     * void monthTenth(AudioTime * currentTime)
     *
     * void month(AudioTime * currentTime)
     *
     * void yearHundreth(AudioTime * currentTime)
     *
     * void yearTenth(AudioTime * currentTime)
     *
     * void year(AudioTime * currentTime)
     *
     * void frame(AudioTime *currentTime)
     *
     */
    class Callback {
    public:
        virtual ~Callback() = default;
        virtual void StartOfFile(AudioTime * currentTime) {/* Stub */}
        virtual void EndOfFile(AudioTime * currentTime) {/* Stub */}

        virtual void nanosecondHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void nanosecondTenth(AudioTime * currentTime) {/* Stub */}
        virtual void nanosecond(AudioTime * currentTime) {/* Stub */}
        virtual void microsecondHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void microsecondTenth(AudioTime * currentTime) {/* Stub */}
        virtual void microsecond(AudioTime * currentTime) {/* Stub */}
        virtual void millisecondHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void millisecondTenth(AudioTime * currentTime) {/* Stub */}
        virtual void millisecond(AudioTime * currentTime) {/* Stub */}
        virtual void secondHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void secondTenth(AudioTime * currentTime) {/* Stub */}
        virtual void second(AudioTime * currentTime) {/* Stub */}
        virtual void minuteHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void minuteTenth(AudioTime * currentTime) {/* Stub */}
        virtual void minute(AudioTime * currentTime) {/* Stub */}
        virtual void hourHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void hourTenth(AudioTime * currentTime) {/* Stub */}
        virtual void hour(AudioTime * currentTime) {/* Stub */}
        virtual void dayHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void dayTenth(AudioTime * currentTime) {/* Stub */}
        virtual void day(AudioTime * currentTime) {/* Stub */}
        virtual void weekHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void weekTenth(AudioTime * currentTime) {/* Stub */}
        virtual void week(AudioTime * currentTime) {/* Stub */}
        virtual void monthHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void monthTenth(AudioTime * currentTime) {/* Stub */}
        virtual void month(AudioTime * currentTime) {/* Stub */}
        virtual void yearHundreth(AudioTime * currentTime) {/* Stub */}
        virtual void yearTenth(AudioTime * currentTime) {/* Stub */}
        virtual void year(AudioTime * currentTime) {/* Stub */}
        virtual void frame(AudioTime *currentTime) {/* Stub */}
    };

    Callback *mTimeCallback = nullptr;
    void setCallback(JNIEnv * ENV, jobject THIS, Callback *timeCallback);
    JavaVM* jvm;
    jobject pThis;

    void calculateNanoseconds(AudioTime *audioTime, AudioTime_Format inputNanoseconds);

    void calculateNanoseconds(AudioTime_Format inputNanoseconds);
};

clock__declare__print_timing_information_function(core_print_time);

#endif //MEDIA_PLAYER_PRO_AUDIOTIME_H
