//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_LOAD_H
#define MEDIA_PLAYER_PRO_LOAD_H

#include <timer.h>
#include "core.h"

NATIVE(void, Oboe, LoadTrackFromAssets)(JNIEnv *env, jobject type, jstring asset, jobject jAssetManager) {
    WAVEFORM_READY = false;
    const char * a = JniHelpers::Strings::newJniStringUTF(env, asset);
    mAssetManager = AAssetManager_fromJava(env, jAssetManager);
    currentAudioTrack = SoundRecording::loadFromAssets(
            mAssetManager,
            a,
            AudioEngine.sampleRate,
            AudioEngine.channelCount
    );
    JniHelpers::Strings::deleteJniStringUTF(&a);
    Mixer.addTrack(currentAudioTrack);
}

NATIVE(void, Oboe, LoadTrackFromPath)(JNIEnv *env, jobject type, jstring path) {
    WAVEFORM_READY = false;
    const char * path_ = JniHelpers::Strings::newJniStringUTF(env, path);
    clock__time__code__block(currentAudioTrack = SoundRecording::loadFromPath(path_, AudioEngine.sampleRate, AudioEngine.channelCount), core_print_time);
    JniHelpers::Strings::deleteJniStringUTF(&path_);
    Mixer.addTrack(currentAudioTrack);
}

#endif //MEDIA_PLAYER_PRO_LOAD_H