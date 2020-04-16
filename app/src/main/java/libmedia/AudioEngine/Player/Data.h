//
// Created by konek on 6/26/2019.
//

#ifndef MEDIA_PLAYER_PRO_DATA_H
#define MEDIA_PLAYER_PRO_DATA_H

#include <native.h>
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

NATIVE(jint, Oboe, underrunCount)(JNIEnv *env, jobject thiz) {
    return AudioEngine.underrunCount;
}

NATIVE(jint, Oboe, previousUnderrunCount)(JNIEnv *env, jobject thiz) {
    return AudioEngine.previousUnderrunCount;
}

NATIVE(jint, Oboe, framesPerBurst)(JNIEnv *env, jobject thiz) {
    return AudioEngine.framesPerBurst;
}

NATIVE(jint, Oboe, bufferSize)(JNIEnv *env, jobject thiz) {
    return AudioEngine.bufferSize;
}

NATIVE(jint, Oboe, bufferCapacity)(JNIEnv *env, jobject thiz) {
    return AudioEngine.bufferCapacity;
}

#endif //MEDIA_PLAYER_PRO_DATA_H
