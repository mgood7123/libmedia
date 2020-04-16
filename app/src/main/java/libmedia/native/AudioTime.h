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
#include "../AudioEngine/audio/SoundRecordingAudioData.h"
#include "../waveform/timestats.h"

class AudioTime {
public:
    double nanosecondsTotal = 0;
    double nanoseconds = 0;
    double nanosecondsPrevious = 0;
    double microsecondsTotal = 0;
    double microseconds = 0;
    double microsecondsPrevious = 0;
    double millisecondsTotal = 0;
    double milliseconds = 0;
    double millisecondsPrevious = 0;
    double secondsTotal = 0;
    double seconds = 0;
    double secondsPrevious = 0;
    double minutesTotal = 0;
    double minutes = 0;
    double minutesPrevious = 0;
    double hoursTotal = 0;
    double hours = 0;
    double hoursPrevious = 0;
    uint64_t currentFrame = 0;
    uint64_t previousFrame = 0;
    uint64_t totalFrames = 0;

    double cb = 0;
    bool StartOfFile = false;
    bool EndOfFile = false;
    bool EndOfFileCalled = false;
    bool initializing = false;

    class divisionValue {
    public:
        double nanoseconds = 1000000000;
        double microseconds = 1000000;
        double milliseconds = 1000;
        double seconds = 1;
        double minutes = 60;
        double hours = 3600;
    };

    class types {
    public:
        int nanoseconds = 1;
        int microseconds = 2;
        int milliseconds = 3;
        int seconds = 4;
        int minutes = 5;
        int hours = 6;
    };

    void update(uint64_t frame, SoundRecordingAudioData *AudioData);
    std::string format(bool truncate);
    void AudioTimeFormat(bool truncate, std::string * string, double type);
    uint64_t toFrame(double value, int type, SoundRecordingAudioData *AudioData);

    /**
     * provided callbacks:
     *
     * void StartOfFile (AudioTime * currentTime)
     *
     * void EndOfFile (AudioTime * currentTime)
     *
     * void nanosecondHundreth (AudioTime * currentTime)
     *
     * void nanosecondTenth (AudioTime * currentTime)
     *
     * void nanosecond (AudioTime * currentTime)
     *
     * void microsecondHundreth (AudioTime * currentTime)
     *
     * void microsecondTenth (AudioTime * currentTime)
     *
     * void microsecond (AudioTime * currentTime)
     *
     * void millisecondHundreth (AudioTime * currentTime)
     *
     * void millisecondTenth (AudioTime * currentTime)
     *
     * void millisecond (AudioTime * currentTime)
     *
     * void secondTenth (AudioTime * currentTime)
     *
     * void second (AudioTime * currentTime)
     *
     * void minuteTenth (AudioTime * currentTime)
     *
     * void minute (AudioTime * currentTime)
     *
     * void hourTenth (AudioTime * currentTime)
     *
     * void hour (AudioTime * currentTime)
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
        virtual void secondTenth(AudioTime * currentTime) {/* Stub */}
        virtual void second(AudioTime * currentTime) {/* Stub */}
        virtual void minuteTenth(AudioTime * currentTime) {/* Stub */}
        virtual void minute(AudioTime * currentTime) {/* Stub */}
        virtual void hourTenth(AudioTime * currentTime) {/* Stub */}
        virtual void hour(AudioTime * currentTime) {/* Stub */}
        virtual void frame(AudioTime *currentTime) {/* Stub */}
    };

    Callback *mTimeCallback = nullptr;
    void setCallback(JNIEnv * ENV, jobject THIS, Callback *timeCallback);
    JavaVM* jvm;
    jobject pThis;
};


#endif //MEDIA_PLAYER_PRO_AUDIOTIME_H
