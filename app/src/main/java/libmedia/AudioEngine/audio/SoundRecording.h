/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_PLAYER_PRO_SOUNDRECORDING_H
#define MEDIA_PLAYER_PRO_SOUNDRECORDING_H

#include <cstdint>
#include <array>

#include <memory>
#include <atomic>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <AudioTime.h>
#include <OboeDebug.h>
#include "SoundRecordingAudioData.h"

extern AudioTime GlobalTime;

class SoundRecording {

public:
    SoundRecordingAudioData * AudioData = nullptr;
    SoundRecording(int16_t *sourceData, SoundRecordingAudioData * audiodata)
            : Audio(sourceData)
            , mTotalFrames(audiodata->totalFrames)
    {
        AudioData = audiodata;
    };
    void renderAudio(int16_t *targetData, int64_t numFrames, SoundRecording *AudioData);
    void resetPlayHead() { mReadFrameIndex = 0; GlobalTime.update(0, AudioData); };
    void seekTo(uint64_t frame) { mReadFrameIndex = frame; };
    void setPlaying(bool isPlaying) { mIsPlaying = isPlaying; };
    void setLooping(bool isLooping) { mIsLooping = isLooping; };

    static SoundRecording * loadFromAssets(AAssetManager *assetManager, const char * filename, int SampleRate, int mChannelCount);
    static SoundRecording *loadFromPath(const char *filename, int SampleRate, int mChannelCount);
    int16_t* Audio = nullptr;

    bool isOscillator = false;
//    bool isOscillator = true;

    uint64_t mReadFrameIndex = 0;
    int64_t mTotalFrames = 0;
    std::atomic<bool> mIsPlaying { false };
    std::atomic<bool> mIsLooping { false };

private:

};

#endif //MEDIA_PLAYER_PRO_SOUNDRECORDING_H
