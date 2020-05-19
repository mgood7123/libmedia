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

NATIVE(jstring, Oboe, getCurrentTime)(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(GlobalTime.format(false));
}

NATIVE(jstring, Oboe, getWaveformLocation)(JNIEnv *env, jobject thiz) {
    LOGE("WAITING FOR WAVEFORM TO BE READY");
    while(!WAVEFORM_READY) {
        // do nothing
    };
    LOGE("WAVEFORM IS READY");
    LOGE("WAVEFORM_LOCATION %s", WAVEFORM_LOCATION.c_str());
    return env->NewStringUTF(WAVEFORM_LOCATION.c_str());
}

NATIVE(jlong, Oboe, getAudioTimingNANO)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->function_duration__renderAudioNANO.count();
}

NATIVE(jlong, Oboe, getAudioTimingMICRO)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->function_duration__renderAudioMICRO.count();
}

NATIVE(jlong, Oboe, getAudioTimingMILLI)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->function_duration__renderAudioMILLI.count();
}

NATIVE(jlong, Oboe, getAudioTimingFormatNANO)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->function_duration__formatNANO.count();
}

NATIVE(jlong, Oboe, getAudioTimingFormatMICRO)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->function_duration__formatMICRO.count();
}

NATIVE(jlong, Oboe, getAudioTimingFormatMILLI)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->function_duration__formatMILLI.count();
}

NATIVE(jlong, Oboe, getAudioTimingChronoNANO)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->function_duration__ChronoNANO.count();
}

NATIVE(jlong, Oboe, getAudioTimingChronoMICRO)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->function_duration__ChronoMICRO.count();
}

NATIVE(jlong, Oboe, getAudioTimingChronoMILLI)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->AudioData->function_duration__ChronoMILLI.count();
}

NATIVE(jlong, Oboe, getCurrentFrame)(JNIEnv *env, jobject thiz) {
    return currentAudioTrack->mReadFrameIndex;
}

#endif //MEDIA_PLAYER_PRO_DATA_H
