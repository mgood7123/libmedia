//i
// Created by macropreprocessor on 18/06/19.
//

#include <vector>
#include <src/common/OboeDebug.h>
#include "AudioTime.h"
#include <MonitorPool.h>

void check(AudioTime *pTime, SoundRecordingAudioData *pData) {
//    AudioTimeCheck(pTime->nanoseconds, pTime->lastNanoseconds, ThisOrElse(pData->nanosecondsPerFrame, 1), 0, 999)
//    AudioTimeCheck(pTime->microseconds, pTime->lastMicroseconds, ThisOrElse(pData->microsecondsPerFrame, 1), 0, 999)
//    AudioTimeCheck(pTime->milliseconds, pTime->lastMilliseconds, ThisOrElse(pData->millisecondsPerFrame, 1), 0, 999)
//    AudioTimeCheck(pTime->seconds, pTime->lastSeconds, ThisOrElse(pData->secondsPerFrame, 1), 0, 60)
//    AudioTimeCheck(pTime->minutes, pTime->lastMinutes, 1, 0, 60)
//    AudioTimeCheck(pTime->hours, pTime->lastHours, 1, 0, 24)
}

void AudioTime::update(uint64_t frame, SoundRecordingAudioData *AudioData) {
    if (initializing) {
        AudioData->nanosecondsPerFrame = nanoseconds;
        AudioData->Initializations->nanosecondsPerFrameInitialized = true;
        AudioData->microsecondsPerFrame = microseconds;
        AudioData->Initializations->microsecondsPerFrameInitialized = true;
        AudioData->millisecondsPerFrame = milliseconds;
        AudioData->Initializations->millisecondsPerFrameInitialized = true;
        AudioData->secondsPerFrame = seconds;
        AudioData->Initializations->secondsPerFrameInitialized = true;
        AudioData->minutesPerFrame = minutes;
        AudioData->Initializations->minutesPerFrameInitialized = true;
        AudioData->hoursPerFrame = hours;
        AudioData->Initializations->hoursPerFrameInitialized = true;
        return;
    }
    if (StartOfFile) {
        if (mTimeCallback != nullptr) mTimeCallback->StartOfFile(this);
        StartOfFile = false;
    }
    if (EndOfFile && !EndOfFileCalled) {
        if (mTimeCallback != nullptr) mTimeCallback->EndOfFile(this);
        EndOfFileCalled = true;
    }

    // TODO: should we migrate this to use Monitor at the cost of information?

    previousFrame = currentFrame;
    currentFrame = frame;
    totalFrames = frame;
    if (currentFrame != previousFrame) if (mTimeCallback != nullptr) mTimeCallback->frame(this);

    nanosecondsTotal =
            (static_cast<double>(frame) * divisionValue().nanoseconds) / static_cast<double>(AudioData->sampleRate);
    nanosecondsPrevious = nanoseconds;
    nanoseconds =
            (static_cast<double>(frame) * divisionValue().nanoseconds) / static_cast<double>(AudioData->sampleRate);

    microsecondsTotal =
            (static_cast<double>(frame) * divisionValue().microseconds) / static_cast<double>(AudioData->sampleRate);
    microsecondsPrevious = microseconds;
    microseconds =
            (static_cast<double>(frame) * divisionValue().microseconds) / static_cast<double>(AudioData->sampleRate);
    if (microseconds != static_cast<double>(0)) {
        if (microseconds < divisionValue().seconds) microseconds = static_cast<double>(0);
        else nanoseconds -= trunc(microseconds) * divisionValue().milliseconds;
    }

    millisecondsTotal =
            (static_cast<double>(frame) * divisionValue().milliseconds) / static_cast<double>(AudioData->sampleRate);
    millisecondsPrevious = milliseconds;
    milliseconds =
            (static_cast<double>(frame) * divisionValue().milliseconds) / static_cast<double>(AudioData->sampleRate);
    if (milliseconds != static_cast<double>(0)) {
        if (milliseconds < divisionValue().seconds) milliseconds = static_cast<double>(0);
        else microseconds -= trunc(milliseconds) * divisionValue().milliseconds;
    }

    secondsTotal = static_cast<double>(frame) / static_cast<double>(AudioData->sampleRate);
    secondsPrevious = seconds;
    seconds = static_cast<double>(frame) / static_cast<double>(AudioData->sampleRate);
    if (seconds != static_cast<double>(0)) {
        if (seconds < divisionValue().seconds) seconds = static_cast<double>(0);
        else milliseconds -= trunc(seconds) * divisionValue().milliseconds;
    }

    minutesTotal = (static_cast<double>(frame) / static_cast<double>(AudioData->sampleRate)) / divisionValue().minutes;
    minutesPrevious = minutes;
    minutes = (static_cast<double>(frame) / static_cast<double>(AudioData->sampleRate)) / divisionValue().minutes;
    if (minutes != static_cast<double>(0)) {
        if (minutes < divisionValue().seconds) minutes = static_cast<double>(0);
        else{
            seconds = (minutes - trunc(minutes)) * divisionValue().minutes;
        }
    }

    hoursTotal = (static_cast<double>(frame) / static_cast<double>(AudioData->sampleRate)) / divisionValue().hours;
    hoursPrevious = hours;
    hours = (static_cast<double>(frame) / static_cast<double>(AudioData->sampleRate)) / divisionValue().hours;
    if (hours != static_cast<double>(0)) {
        if (hours < divisionValue().seconds) hours = static_cast<double>(0);
        else minutes = (hours - trunc(hours)) * divisionValue().minutes;
    }

    // wrap tp zero
    if (hours == static_cast<double>(24)) hours = static_cast<double>(0);

    if (AudioData != nullptr) {
        if (
                !initializing &&
                (
                        !AudioData->Initializations->nanosecondsPerFrameInitialized ||
                        !AudioData->Initializations->microsecondsPerFrameInitialized ||
                        !AudioData->Initializations->millisecondsPerFrameInitialized ||
                        !AudioData->Initializations->secondsPerFrameInitialized ||
                        !AudioData->Initializations->minutesPerFrameInitialized ||
                        !AudioData->Initializations->hoursPerFrameInitialized
                )
         ) {
            initializing = true;
            update(1, AudioData);
            initializing = false;
        }

        // round down for callbacks

        if (floor(nanoseconds) != floor(nanosecondsPrevious)) {
            if (EndOfFile) {
                EndOfFile = false;
                EndOfFileCalled = false;
            }
            if (mTimeCallback != nullptr) {
                if (!(static_cast<int>(floor(nanoseconds)) % 100)) mTimeCallback->nanosecondHundreth(this);
                if (!(static_cast<int>(floor(nanoseconds)) % 10)) mTimeCallback->nanosecondTenth(this);
                mTimeCallback->nanosecond(this);
            }
        }
        if (floor(microseconds) != floor(microsecondsPrevious)) {
            if (mTimeCallback != nullptr) {
                if (!(static_cast<int>(floor(microseconds)) % 100)) mTimeCallback->microsecondHundreth(this);
                if (!(static_cast<int>(floor(microseconds)) % 10)) mTimeCallback->microsecondTenth(this);
                mTimeCallback->microsecond(this);
            }
        }
        if (floor(milliseconds) != floor(millisecondsPrevious)) {
            if (mTimeCallback != nullptr) {
                if (!(static_cast<int>(floor(milliseconds)) % 100)) mTimeCallback->millisecondHundreth(this);
                if (!(static_cast<int>(floor(milliseconds)) % 10)) mTimeCallback->millisecondTenth(this);
                mTimeCallback->millisecond(this);
            }
        }
        if (floor(seconds) != floor(secondsPrevious)) {
            if (mTimeCallback != nullptr) {
                if (!(static_cast<int>(floor(seconds)) % 10)) mTimeCallback->secondTenth(this);
                mTimeCallback->second(this);
            }
        }
        if (floor(minutes) != floor(minutesPrevious)) {
            if (mTimeCallback != nullptr) {
                if (!(static_cast<int>(floor(minutes)) % 10)) mTimeCallback->minuteTenth(this);
                mTimeCallback->minute(this);
            }
        }
        if (floor(hours) != floor(hoursPrevious)) {
            if (mTimeCallback != nullptr) {
                if (!(static_cast<int>(floor(hours)) % 10)) mTimeCallback->hourTenth(this);
                mTimeCallback->hour(this);
            }
        }
    }
}

void AudioTime::setCallback(JNIEnv * ENV, jobject THIS, AudioTime::Callback *timeCallback) {
    mTimeCallback = timeCallback;
    ENV->GetJavaVM(&jvm);
    pThis = THIS;
}

void AudioTime::AudioTimeFormat(bool truncate, std::string *string, double type) {
    if (type != 0) {
        if ((*string).length() != 0) *string += ":";
        if (truncate) *string += std::to_string(static_cast<uint64_t>(trunc(type)));
        else *string += std::to_string(type);
    }
}

std::string AudioTime::format(bool truncate) {
    std::string str;
    AudioTimeFormat(truncate, &str, hours);
    AudioTimeFormat(truncate, &str, minutes);
    AudioTimeFormat(truncate, &str, seconds);
    AudioTimeFormat(truncate, &str, milliseconds);
    AudioTimeFormat(truncate, &str, microseconds);
    AudioTimeFormat(truncate, &str, nanoseconds);
    return str;
}

uint64_t AudioTime::toFrame(double value, int type, SoundRecordingAudioData *AudioData) {
    if (type == AudioTime::types().nanoseconds)
        return static_cast<uint64_t>((value * AudioData->sampleRate) / divisionValue().nanoseconds);
    if (type == AudioTime::types().microseconds)
        return static_cast<uint64_t>((value * AudioData->sampleRate) / divisionValue().microseconds);
    if (type == AudioTime::types().milliseconds)
        return static_cast<uint64_t>((value * AudioData->sampleRate) / divisionValue().milliseconds);
    if (type == AudioTime::types().seconds)
        return static_cast<uint64_t>(value * AudioData->sampleRate);
    if (type == AudioTime::types().minutes)
        return static_cast<uint64_t>(value * divisionValue().minutes * AudioData->sampleRate);
    if (type == AudioTime::types().hours)
        return static_cast<uint64_t>(value * divisionValue().hours * AudioData->sampleRate);
    return 0;
}

void AUDIOTIMECHECK(char *name, int64_t now, int64_t last, int64_t increment, int64_t min, int64_t max) {
    if (abs(now-last) > increment && (now != min && last != max)) {
        LOGE("%s is %ld, expected %ld", name, now, last+increment);
        exit(-1);
    }
}
