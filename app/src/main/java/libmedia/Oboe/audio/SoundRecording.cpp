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

#include "SoundRecording.h"
#include "../../waveform/AudioTools.h"
#include "../../shell/env.h"
#include "../../shell/regex_str.h"
#include <src/common/OboeDebug.h>
#include <cmath>
#include <native.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <asm/fcntl.h>
#include <fcntl.h>

extern AudioTime GlobalTime;

void SoundRecording::renderAudio(int16_t *targetData, int64_t totalFrames, SoundRecording *Audio){
    SoundRecordingAudioData * AudioData = Audio->AudioData;
    if (mIsPlaying) {

        // Check whether we're about to reach the end of the recording
        if (!mIsLooping && mReadFrameIndex + totalFrames >= mTotalFrames) {
            totalFrames = mTotalFrames - mReadFrameIndex;
            mIsPlaying = false;
        }

//        LOGW("SoundRecording::renderAudio: rendering %d frames with data", totalFrames);

        if (mReadFrameIndex == 0) {
            GlobalTime.StartOfFile = true;
            GlobalTime.update(mReadFrameIndex, AudioData);
//            LOGW("SoundRecording::renderAudio: AudioTime in milliseconds = %lld", GlobalTime.milliseconds.count());
        }
        for (int i = 0; i < totalFrames; ++i) {
            for (int j = 0; j < AudioData->channelCount; ++j) {
                targetData[(i * AudioData->channelCount) + j] = Audio->Audio[(mReadFrameIndex * AudioData->channelCount) + j];
            }
            // Increment and handle wraparound
            if (++mReadFrameIndex >= mTotalFrames) {
                GlobalTime.EndOfFile = true;
                GlobalTime.update(mReadFrameIndex, AudioData);
                mReadFrameIndex = 0;
            } else {
                GlobalTime.update(mReadFrameIndex, AudioData);
            }
//            LOGW("SoundRecording::renderAudio: mReadFrameIndex = %d", mReadFrameIndex);
//            LOGW("SoundRecording::renderAudio: AudioTime in milliseconds = %lld", GlobalTime.milliseconds.count());
        }
    } else {
//        LOGW("SoundRecording::renderAudio: rendering %d frames with zero", totalFrames);
        // fill with zeros to output silence
        for (int i = 0; i < totalFrames * AudioData->channelCount; ++i) {
            targetData[i] = 0;
        }
    }
}

extern const int16_t *WAVEFORMAUDIODATA;
extern uint64_t WAVEFORMAUDIODATATOTALFRAMES;
char * TEMPDIR;

NATIVE(void, Oboe, SetTempDir)(JNIEnv *env, jobject type, jstring dir) {
    jboolean val;
    TEMPDIR = const_cast<char *>(env->GetStringUTFChars(dir, &val));
}

void resample(int inSampleRate, int outSampleRate, const char * inFilename, char ** out, size_t * outsize) {
    extern int main(int argc, char * argv[]);
    str_new(outfile);
    str_insert_string(outfile, TEMPDIR);
    str_insert_string(outfile, "/INFILE.raw");
    env_t argv = env__new();
    argv = env__add_allow_duplicates(argv, "ReSampler");
    argv = env__add_allow_duplicates(argv, "-i");
    argv = env__add_allow_duplicates(argv, inFilename);
    argv = env__add_allow_duplicates(argv, "--raw-input");
    argv = env__add_allow_duplicates(argv, std::to_string(inSampleRate).c_str());
    argv = env__add_allow_duplicates(argv, "16");
    argv = env__add_allow_duplicates(argv, "2");
    argv = env__add_allow_duplicates(argv, "-o");
    argv = env__add_allow_duplicates(argv, outfile.string);
    argv = env__add_allow_duplicates(argv, "-r");
    argv = env__add_allow_duplicates(argv, std::to_string(outSampleRate).c_str());
    argv = env__add_allow_duplicates(argv, "-b");
    argv = env__add_allow_duplicates(argv, "16"); // argv[13]
    argv = env__add_allow_duplicates(argv, "--showStages");
    argv = env__add_allow_duplicates(argv, "-mt");
    argv = env__add_allow_duplicates(argv, "--noTempFile");
    double s = now_ms();
    LOGE("Started conversion at %G milliseconds", s);
    main(env__size(argv), argv);
    double e = now_ms();
    LOGE("Ended conversion at %G milliseconds", e);
    LOGE("TIME took %G milliseconds", e - s);

    // read the file into memory
    *outsize = read__(const_cast<char *>(outfile.string), out);

    LOGE("%s file size: %zu", outfile.string, *outsize);
    str_free(outfile);
    env__free(argv);
}

SoundRecording * SoundRecording::loadFromPath(const char *filename, int SampleRate, int mChannelCount) {
    char * out = nullptr;
    size_t outsize = 0;
    resample(SampleRate, 48000, filename, &out, &outsize);

    const uint64_t totalFrames = outsize / (2 * mChannelCount);
    WAVEFORMAUDIODATATOTALFRAMES = totalFrames;
    WAVEFORMAUDIODATA = reinterpret_cast<const int16_t *>(out);

    SoundRecordingAudioData * AudioData = new SoundRecordingAudioData(totalFrames, mChannelCount, SampleRate);
    AudioTime * allFrames = new AudioTime();
    allFrames->update(totalFrames, AudioData);
    LOGD("Opened backing track");
    LOGD("length in human time:                              %s", allFrames->format(true).c_str());
    LOGD("length in nanoseconds:                             %G", allFrames->nanosecondsTotal);
    LOGD("length in microseconds:                            %G", allFrames->microsecondsTotal);
    LOGD("length in milliseconds:                            %G", allFrames->millisecondsTotal);
    LOGD("length in seconds:                                 %G", allFrames->secondsTotal);
    LOGD("length in minutes:                                 %G", allFrames->minutesTotal);
    LOGD("length in hours:                                   %G", allFrames->hoursTotal);
    LOGD("bytes:                                             %ld", outsize);
    LOGD("frames:                                            %ld", totalFrames);
    LOGD("sample rate:                                       %d", SampleRate);
    LOGD("length of 1 frame at %d sample rate:", SampleRate);
    LOGD("Human Time:                                        %s", AudioData->TimeTruncated);
    LOGD("Nanoseconds:                                       %G", AudioData->nanosecondsPerFrame);
    LOGD("Microseconds:                                      %G", AudioData->microsecondsPerFrame);
    LOGD("Milliseconds:                                      %G", AudioData->millisecondsPerFrame);
    LOGD("Seconds:                                           %G", AudioData->secondsPerFrame);
    LOGD("Minutes:                                           %G", AudioData->minutesPerFrame);
    LOGD("Hours:                                             %G", AudioData->hoursPerFrame);
    return new SoundRecording(reinterpret_cast<int16_t *>(out), AudioData);
}

SoundRecording * SoundRecording::loadFromAssets(AAssetManager *assetManager, const char *filename, int SampleRate, int mChannelCount) {

    // Load the backing track
    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);

    if (asset == nullptr){
        LOGE("Failed to open track, filename %s", filename);
        return nullptr;
    }

    // Get the length of the track (we assume it is stereo 48kHz)
    uint64_t trackSize = static_cast<uint64_t>(AAsset_getLength(asset));

    // Load it into memory
    const int16_t *audioBuffer = static_cast<const int16_t*>(AAsset_getBuffer(asset));
    if (audioBuffer == nullptr){
        LOGE("Could not get buffer for track");
        return nullptr;
    }
    const int actualSampleRate = 48000;
    const int actualChannelCount = 2;

    const uint64_t totalFrames = trackSize / (2 * actualChannelCount);
    WAVEFORMAUDIODATATOTALFRAMES = totalFrames;
    WAVEFORMAUDIODATA = audioBuffer;

    SoundRecordingAudioData * AudioData = new SoundRecordingAudioData(totalFrames, mChannelCount, SampleRate);
    AudioTime * allFrames = new AudioTime();
    allFrames->update(totalFrames, AudioData);
    LOGD("Opened backing track");
    LOGD("length in human time:                              %s", allFrames->format(true).c_str());
    LOGD("length in nanoseconds:                             %G", allFrames->nanosecondsTotal);
    LOGD("length in microseconds:                            %G", allFrames->microsecondsTotal);
    LOGD("length in milliseconds:                            %G", allFrames->millisecondsTotal);
    LOGD("length in seconds:                                 %G", allFrames->secondsTotal);
    LOGD("length in minutes:                                 %G", allFrames->minutesTotal);
    LOGD("length in hours:                                   %G", allFrames->hoursTotal);
    LOGD("bytes:                                             %ld", trackSize);
    LOGD("frames:                                            %ld", totalFrames);
    LOGD("sample rate:                                       %d", SampleRate);
    LOGD("length of 1 frame at %d sample rate:", SampleRate);
    LOGD("Human Time:                                        %s", AudioData->TimeTruncated);
    LOGD("Nanoseconds:                                       %G", AudioData->nanosecondsPerFrame);
    LOGD("Microseconds:                                      %G", AudioData->microsecondsPerFrame);
    LOGD("Milliseconds:                                      %G", AudioData->millisecondsPerFrame);
    LOGD("Seconds:                                           %G", AudioData->secondsPerFrame);
    LOGD("Minutes:                                           %G", AudioData->minutesPerFrame);
    LOGD("Hours:                                             %G", AudioData->hoursPerFrame);
    return new SoundRecording(const_cast<int16_t *>(audioBuffer), AudioData);
}
