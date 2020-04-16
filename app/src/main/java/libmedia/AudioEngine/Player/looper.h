//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_LOOPER_H
#define MEDIA_PLAYER_PRO_LOOPER_H

#include "core.h"

NATIVE(void, Oboe, Loop)(JNIEnv *env, jobject type, jboolean value) {
    currentAudioTrack->setLooping(value);
}

NATIVE(void, Oboe, Looper)(JNIEnv *env, jobject type, jdouble start, jdouble end, jint timing) {
    Looper.type = timing;
    Looper.start = start;
    Looper.end = end;
}

#endif //MEDIA_PLAYER_PRO_LOOPER_H
