//i
// Created by macropreprocessor on 18/06/19.
//

#include <vector>
#include <OboeDebug.h>
#include "AudioTime.h"

void AudioTime::calculateNanoseconds(AudioTime_Format inputNanoseconds) {
    calculateNanoseconds(this, inputNanoseconds);
}

void AudioTime::calculateNanoseconds(AudioTime * audioTime, AudioTime_Format inputNanoseconds) {
    // duration::time is unsigned, while chrono::time is signed

    duration::nanoseconds nanoseconds = duration::nanoseconds (inputNanoseconds);
    audioTime->nanosecondsPrevious = audioTime->nanoseconds;
    audioTime->nanosecondsTotal = nanoseconds.count();

    duration::microseconds microseconds = std::chrono::duration_cast<duration::microseconds>(nanoseconds);
    audioTime->microsecondsPrevious = audioTime->microseconds;
    audioTime->microsecondsTotal = microseconds.count();
    nanoseconds -= std::chrono::duration_cast<duration::nanoseconds>(microseconds);
    audioTime->nanoseconds = nanoseconds.count();

    duration::milliseconds milliseconds = std::chrono::duration_cast<duration::milliseconds>(microseconds);
    audioTime->millisecondsPrevious = audioTime->milliseconds;
    audioTime->millisecondsTotal = milliseconds.count();
    microseconds -= std::chrono::duration_cast<duration::milliseconds>(microseconds);
    audioTime->microseconds = microseconds.count();

    duration::seconds seconds = std::chrono::duration_cast<duration::seconds>(milliseconds);
    audioTime->secondsPrevious = audioTime->seconds;
    audioTime->secondsTotal = seconds.count();
    milliseconds -= std::chrono::duration_cast<duration::milliseconds>(seconds);
    audioTime->milliseconds = milliseconds.count();

    duration::minutes minutes = std::chrono::duration_cast<duration::minutes>(seconds);
    audioTime->minutesPrevious = audioTime->minutes;
    audioTime->minutesTotal = minutes.count();
    seconds -= std::chrono::duration_cast<duration::seconds>(minutes);
    audioTime->seconds = seconds.count();

    duration::hours hours = std::chrono::duration_cast<duration::hours>(minutes);
    audioTime->hoursPrevious = audioTime->hours;
    audioTime->hoursTotal = hours.count();
    minutes -= std::chrono::duration_cast<duration::minutes>(hours);
    audioTime->minutes = minutes.count();

//    typedef date::days days;
//    typedef date::weeks weeks;
//    typedef date::months months;
//    typedef date::years years;

    duration::days days = std::chrono::duration_cast<duration::days>(hours);
    audioTime->daysPrevious = audioTime->days;
    audioTime->daysTotal = days.count();
    hours -= std::chrono::duration_cast<duration::hours>(days);
    audioTime->hours = hours.count();

    duration::weeks weeks = std::chrono::duration_cast<duration::weeks>(days);
    audioTime->weeksPrevious = audioTime->weeks;
    audioTime->weeksTotal = weeks.count();
    days -= std::chrono::duration_cast<duration::days>(weeks);
    audioTime->days = days.count();

    duration::months months = std::chrono::duration_cast<duration::months>(weeks);
    audioTime->monthsPrevious = audioTime->months;
    audioTime->monthsTotal = months.count();
    weeks -= std::chrono::duration_cast<duration::weeks>(months);
    audioTime->weeks = weeks.count();

    duration::years years = std::chrono::duration_cast<duration::years>(months);
    audioTime->yearsPrevious = audioTime->years;
    audioTime->yearsTotal = years.count();
    audioTime->years = audioTime->yearsTotal;
    months -= std::chrono::duration_cast<duration::months>(years);
    audioTime->months = months.count();
}

void AudioTime::update(uint64_t frame, SoundRecordingAudioData *AudioData) {

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

    std::chrono::steady_clock::time_point start, end;
    if (includeTimingInformation) start = std::chrono::steady_clock::now();

    AudioTime_Format nanos = static_cast<AudioTime_Format>(frame) * divisionValue().nanoseconds;
    AudioTime_Format sampleRate = static_cast<AudioTime_Format>(AudioData->sampleRate);

    calculateNanoseconds(nanos / sampleRate);

    if (includeTimingInformation) end = std::chrono::steady_clock::now();

    if (AudioData != nullptr) {
        if (includeTimingInformation) {
            AudioData->function_duration__ChronoNANO = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    end - start);
            AudioData->function_duration__ChronoMICRO = std::chrono::duration_cast<std::chrono::microseconds>(
                    AudioData->function_duration__ChronoNANO);
            AudioData->function_duration__ChronoMILLI = std::chrono::duration_cast<std::chrono::milliseconds>(
                    AudioData->function_duration__ChronoMICRO);
        }
        if (AudioData->Initializations->ShouldInitialize) {
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
            AudioData->daysPerFrame = days;
            AudioData->Initializations->daysPerFrameInitialized = true;
            AudioData->weeksPerFrame = weeks;
            AudioData->Initializations->weeksPerFrameInitialized = true;
            AudioData->monthsPerFrame = months;
            AudioData->Initializations->monthsPerFrameInitialized = true;
            AudioData->yearsPerFrame = years;
            AudioData->Initializations->yearsPerFrameInitialized = true;
            AudioData->TimeHumanizedPerFrame = format(true, AudioData);
            AudioData->TimeNormalPerFrame = format(false, AudioData);
            AudioData->Initializations->ShouldInitialize = false;
        }
        AudioData->TimeHumanized = format(true, AudioData);
        AudioData->TimeNormal = format(false, AudioData);

        if (EndOfFile) {
            EndOfFile = false;
            EndOfFileCalled = false;
        }

        if (executeCallbacks) {
            if (this->nanoseconds != this->nanosecondsPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->nanoseconds % 100)) mTimeCallback->nanosecondHundreth(this);
                    if (!(this->nanoseconds % 10)) mTimeCallback->nanosecondTenth(this);
                    mTimeCallback->nanosecond(this);
                }
            }
            if (this->microseconds != this->microsecondsPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->microseconds % 100)) mTimeCallback->microsecondHundreth(this);
                    if (!(this->microseconds % 10)) mTimeCallback->microsecondTenth(this);
                    mTimeCallback->microsecond(this);
                }
            }
            if (this->milliseconds != this->millisecondsPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->milliseconds % 100)) mTimeCallback->millisecondHundreth(this);
                    if (!(this->milliseconds % 10)) mTimeCallback->millisecondTenth(this);
                    mTimeCallback->millisecond(this);
                }
            }
            if (this->seconds != this->secondsPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->seconds % 100)) mTimeCallback->secondHundreth(this);
                    if (!(this->seconds % 10)) mTimeCallback->secondTenth(this);
                    mTimeCallback->second(this);
                }
            }
            if (this->minutes != this->minutesPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->minutes % 100)) mTimeCallback->minuteHundreth(this);
                    if (!(this->minutes % 10)) mTimeCallback->minuteTenth(this);
                    mTimeCallback->minute(this);
                }
            }
            if (this->hours != this->hoursPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->hours % 100)) mTimeCallback->hourHundreth(this);
                    if (!(this->hours % 10)) mTimeCallback->hourTenth(this);
                    mTimeCallback->hour(this);
                }
            }
            if (this->days != this->daysPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->days % 100)) mTimeCallback->dayHundreth(this);
                    if (!(this->days % 10)) mTimeCallback->dayTenth(this);
                    mTimeCallback->day(this);
                }
            }
            if (this->weeks != this->weeksPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->weeks % 100)) mTimeCallback->weekHundreth(this);
                    if (!(this->weeks % 10)) mTimeCallback->weekTenth(this);
                    mTimeCallback->week(this);
                }
            }
            if (this->months != this->monthsPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->months % 100)) mTimeCallback->monthHundreth(this);
                    if (!(this->months % 10)) mTimeCallback->monthTenth(this);
                    mTimeCallback->month(this);
                }
            }
            if (this->years != this->yearsPrevious) {
                if (mTimeCallback != nullptr) {
                    if (!(this->years % 100)) mTimeCallback->yearHundreth(this);
                    if (!(this->years % 10)) mTimeCallback->yearTenth(this);
                    mTimeCallback->year(this);
                }
            }
        }
    }
}

void AudioTime::setCallback(JNIEnv * ENV, jobject THIS, AudioTime::Callback *timeCallback) {
    mTimeCallback = timeCallback;
    ENV->GetJavaVM(&jvm);
    pThis = THIS;
}

// use pre-allocated buffer to improve performance
// years:months:weeks:days:hours:minutes:seconds:milliseconds:microseconds:nanoseconds

char format_buffer[35];
bool buffer_set = false;

size_t pointer; // to track format_buffer memset/memcpy

char * AudioTime::format(bool include_human_time_markings) {
    return format(include_human_time_markings, nullptr);
}

char * AudioTime::format(bool include_human_time_markings, SoundRecordingAudioData * AudioData) {
    int pointer = 0;
    if (!buffer_set) {
        const char * buf = "0000:00:00:00:00:00:00:000:000:000"; // 35 chars long including null-term
        for (int i = 0; i != 36; i++) format_buffer[i] = buf[i]; // break after copying null-term
        buffer_set = true;
    }
    std::chrono::steady_clock::time_point start, end;
    if (includeTimingInformation) start = std::chrono::steady_clock::now();
    ATM(include_human_time_markings, format_buffer, pointer, years, 4);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, months, 2);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, weeks, 2);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, days, 2);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, hours, 2);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, minutes, 2);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, seconds, 2);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, milliseconds, 3);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, microseconds, 3);
    format_buffer[pointer++] = ':';
    ATM(include_human_time_markings, format_buffer, pointer, nanoseconds, 3);
    if (includeTimingInformation) end = std::chrono::steady_clock::now();
    // Calculating total time taken by the function.
    if (AudioData != nullptr && includeTimingInformation) {
        AudioData->function_duration__formatNANO = std::chrono::duration_cast<std::chrono::nanoseconds>(
                end - start);
        AudioData->function_duration__formatMICRO = std::chrono::duration_cast<std::chrono::microseconds>(
                AudioData->function_duration__formatNANO);
        AudioData->function_duration__formatMILLI = std::chrono::duration_cast<std::chrono::milliseconds>(
                AudioData->function_duration__formatMICRO);
    }
    return format_buffer;
}

uint64_t AudioTime::toFrame(AudioTime_Format value, int type, SoundRecordingAudioData *AudioData) {
    if (type == AudioTime::types().nanoseconds)
        return ((value * AudioData->sampleRate) / divisionValue().nanoseconds);
    if (type == AudioTime::types().microseconds)
        return ((value * AudioData->sampleRate) / divisionValue().microseconds);
    if (type == AudioTime::types().milliseconds)
        return ((value * AudioData->sampleRate) / divisionValue().milliseconds);
    if (type == AudioTime::types().seconds)
        return (value * AudioData->sampleRate);
    if (type == AudioTime::types().minutes)
        return (value * divisionValue().minutes * AudioData->sampleRate);
    if (type == AudioTime::types().hours)
        return (value * divisionValue().hours * AudioData->sampleRate);
    if (type == AudioTime::types().days)
        return (value * divisionValue().days * AudioData->sampleRate);
    if (type == AudioTime::types().weeks)
        return (value * divisionValue().weeks * AudioData->sampleRate);
    if (type == AudioTime::types().months)
        return (value * divisionValue().months * AudioData->sampleRate);
    if (type == AudioTime::types().years)
        return (value * divisionValue().years * AudioData->sampleRate);
    return 0;
}