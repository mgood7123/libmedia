//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_CORE_H
#define MEDIA_PLAYER_PRO_CORE_H

#include <OboeDebug.h>
#include <native.h>
#include "../AudioEngine.h"
#include <android/asset_manager_jni.h>


AudioEngine AudioEngine;
AudioTime GlobalTime;
Mixer Mixer;
AAssetManager *mAssetManager {nullptr};
SoundRecording *currentAudioTrack {nullptr};

#include "internal/looper.h"
Looper Looper;

#include "internal/callback.h"
CALLBACK CALLBACK;

#endif //MEDIA_PLAYER_PRO_CORE_H
