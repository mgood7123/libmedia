//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_INIT_H
#define MEDIA_PLAYER_PRO_INIT_H

//
// Created by macropreprocessor on 16/06/19.
//

#include "core.h"

// .RAW extension:
// As there is no header, compatible audio players require information from the user that would
// normally be stored in a header, such as the following:
// encoding, sample rate, number of bits used per sample, and the number of channels.

NATIVE(void, Oboe, Init)(JNIEnv *env, jobject type, jint sampleRate, jint framesPerBurst) {
    LOGW("Oboe_Init: Creating and Opening AudioStream");
    AudioEngine.CreateAndOpenStream(sampleRate, framesPerBurst, 2);
    GlobalTime.setCallback(env, type, &CALLBACK);
}

NATIVE(void, Oboe, Cleanup)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: closing AudioStream");
    AudioEngine.StopStream();
    AudioEngine.FlushStream();
    currentAudioTrack = {nullptr};
    LOGW("Oboe_Init: closed AudioStream");
}


#endif //MEDIA_PLAYER_PRO_INIT_H
