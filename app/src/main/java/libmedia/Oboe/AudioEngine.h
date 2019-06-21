//
// Created by macropreprocessor on 17/06/19.
//

#ifndef MEDIA_PLAYER_PRO_AUDIOENGINE_H
#define MEDIA_PLAYER_PRO_AUDIOENGINE_H
#include <oboe/Oboe.h>

#include <AudioTime.h>

extern AudioTime GlobalTime;

class AudioEngine : public oboe::AudioStreamCallback {
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames);
};

#include "audio/Mixer.h"

#endif //MEDIA_PLAYER_PRO_AUDIOENGINE_H
