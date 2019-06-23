//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_PLAYBACK_H
#define MEDIA_PLAYER_PRO_PLAYBACK_H

#include "core.h"


NATIVE(void, Oboe, Play)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: requesting Start");
    oboe::Result result = stream->requestStart();
    LOGW("Oboe_Init: requested Start");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to start AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    currentAudioTrack->setPlaying(true);
}

NATIVE(void, Oboe, Pause)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: requesting Pause");
    oboe::Result result = stream->requestPause();
    LOGW("Oboe_Init: requested Pause");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to pause AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    currentAudioTrack->setPlaying(false);
}

NATIVE(void, Oboe, Stop)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: requesting Stop");
    oboe::Result result = stream->requestStop();
    LOGW("Oboe_Init: requested Stop");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to stop AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    currentAudioTrack->setPlaying(true);
    currentAudioTrack->resetPlayHead();
}

#endif //MEDIA_PLAYER_PRO_PLAYBACK_H
