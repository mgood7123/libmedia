//
// Created by macropreprocessor on 18/06/19.
//

#ifndef MEDIA_PLAYER_PRO_SOUNDRECORDINGAUDIODATA_H
#define MEDIA_PLAYER_PRO_SOUNDRECORDINGAUDIODATA_H

#include <cstdint>
#include <AudioTime.h>

class SoundRecordingAudioData {
public:
    SoundRecordingAudioData(const uint64_t totalFrames, const int channelCount, const int sampleRate)
            : totalFrames(totalFrames), channelCount(channelCount), sampleRate(sampleRate) {
        Initializations = new class Initializations();
    }

    const uint64_t totalFrames;
    int32_t channelCount;
    int32_t sampleRate;
    class Initializations {
    public:
        bool nanosecondsPerFrameInitialized = false;
        bool microsecondsPerFrameInitialized = false;
        bool millisecondsPerFrameInitialized = false;
        bool secondsPerFrameInitialized = false;
        bool minutesPerFrameInitialized = false;
        bool hoursPerFrameInitialized = false;
        bool daysPerFrameInitialized = false;
        bool weeksPerFrameInitialized = false;
        bool monthsPerFrameInitialized = false;
        bool yearsPerFrameInitialized = false;
        bool ShouldInitialize = false;
    };
    Initializations *Initializations;
    uint64_t nanosecondsPerFrame = 0;
    uint64_t microsecondsPerFrame = 0;
    uint64_t millisecondsPerFrame = 0;
    uint64_t secondsPerFrame = 0;
    uint64_t minutesPerFrame = 0;
    uint64_t hoursPerFrame = 0;
    uint64_t daysPerFrame = 0;
    uint64_t weeksPerFrame = 0;
    uint64_t monthsPerFrame = 0;
    uint64_t yearsPerFrame = 0;
    char * TimeHumanized;
    char * TimeNormal;
    char * TimeHumanizedPerFrame;
    char * TimeNormalPerFrame;

    std::chrono::nanoseconds function_duration__formatNANO = std::chrono::nanoseconds(0);
    std::chrono::microseconds function_duration__formatMICRO = std::chrono::microseconds(0);
    std::chrono::milliseconds function_duration__formatMILLI = std::chrono::milliseconds(0);
    std::chrono::nanoseconds function_duration__ChronoNANO = std::chrono::nanoseconds(0);
    std::chrono::microseconds function_duration__ChronoMICRO = std::chrono::microseconds(0);
    std::chrono::milliseconds function_duration__ChronoMILLI = std::chrono::milliseconds(0);

};

#endif //MEDIA_PLAYER_PRO_SOUNDRECORDINGAUDIODATA_H
