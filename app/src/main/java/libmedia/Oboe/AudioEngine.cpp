//
// Created by macropreprocessor on 17/06/19.
//

#include <include/oboe/Oboe.h>
#include <src/common/OboeDebug.h>
#include "AudioEngine.h"
#include "Generator.h"
#include <AudioTime.h>

AudioTime GlobalTime;

extern Mixer Mixer;
//extern SoundRecording *currentAudioTrack;

oboe::DataCallbackResult AudioEngine::onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames) {

    Mixer.renderAudio(static_cast<int16_t *>(audioData), numFrames);
// play the sound directly for now
//    currentAudioTrack->renderAudio(static_cast<int16_t *>(audioData), numFrames, currentAudioTrack);
    return oboe::DataCallbackResult::Continue;
//    return oboe::DataCallbackResult::Stop;
}
