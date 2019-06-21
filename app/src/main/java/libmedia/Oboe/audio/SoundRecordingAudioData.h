//
// Created by macropreprocessor on 18/06/19.
//

#ifndef MEDIA_PLAYER_PRO_SOUNDRECORDINGAUDIODATA_H
#define MEDIA_PLAYER_PRO_SOUNDRECORDINGAUDIODATA_H

#include <cstdint>

class SoundRecordingAudioData {
public:
    SoundRecordingAudioData(const uint64_t totalFrames, const int64_t channelCount, const int64_t sampleRate)
            : totalFrames(totalFrames), channelCount(channelCount), sampleRate(sampleRate) {
        Initializations = new class Initializations();
    }

public:
    const uint64_t totalFrames;
    const int64_t channelCount;
    const int64_t sampleRate;
    class Initializations {
    public:
        bool nanosecondsPerFrameInitialized = false;
        bool microsecondsPerFrameInitialized = false;
        bool millisecondsPerFrameInitialized = false;
        bool secondsPerFrameInitialized = false;
        bool minutesPerFrameInitialized = false;
        bool hoursPerFrameInitialized = false;
    };
    Initializations *Initializations;
    double nanosecondsPerFrame = 0;
    double microsecondsPerFrame = 0;
    double millisecondsPerFrame = 0;
    double secondsPerFrame = 0;
    double minutesPerFrame = 0;
    double hoursPerFrame = 0;
};

#endif //MEDIA_PLAYER_PRO_SOUNDRECORDINGAUDIODATA_H
