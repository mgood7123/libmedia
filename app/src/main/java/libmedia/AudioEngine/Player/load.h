//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_LOAD_H
#define MEDIA_PLAYER_PRO_LOAD_H

#include "core.h"

NATIVE(void, Oboe, LoadTrackFromAssets)(JNIEnv *env, jobject type, jstring asset, jobject jAssetManager) {
    jboolean val;
    const char * a = env->GetStringUTFChars(asset, &val);
    mAssetManager = AAssetManager_fromJava(env, jAssetManager);
    currentAudioTrack = SoundRecording::loadFromAssets(
            mAssetManager,
            a,
            AudioEngine.sampleRate,
            AudioEngine.channelCount
    );
    env->ReleaseStringUTFChars(asset, a);
    Mixer.addTrack(currentAudioTrack);
}

NATIVE(void, Oboe, LoadTrackFromPath)(JNIEnv *env, jobject type, jstring path) {
    jboolean val;
    currentAudioTrack = SoundRecording::loadFromPath(
            env->GetStringUTFChars(path, &val),
            AudioEngine.sampleRate,
            AudioEngine.channelCount
    );
    Mixer.addTrack(currentAudioTrack);
}

#endif //MEDIA_PLAYER_PRO_LOAD_H
