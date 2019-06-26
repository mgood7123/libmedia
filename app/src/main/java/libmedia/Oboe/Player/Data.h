//
// Created by konek on 6/26/2019.
//

#ifndef MEDIA_PLAYER_PRO_DATA_H
#define MEDIA_PLAYER_PRO_DATA_H

#include "../../native/native.h"
#include "core.h"

NATIVE(jlong, Oboe, SampleCount)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->totalFrames;
}

NATIVE(jshort, Oboe, SampleIndex)(JNIEnv *env, jobject thiz, jlong index) {
    return currentAudioTrack->Audio[index];
}

NATIVE(jint, Oboe, SampleRate)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->sampleRate;
}

NATIVE(jint, Oboe, ChannelCount)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->channelCount;
}

#endif //MEDIA_PLAYER_PRO_DATA_H
