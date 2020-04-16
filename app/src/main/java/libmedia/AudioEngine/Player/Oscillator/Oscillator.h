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


#ifndef LOWLATENCYAUDIO_OSCILLATOR_H
#define LOWLATENCYAUDIO_OSCILLATOR_H


#include <cstdint>
#include <atomic>
#include <math.h>
#include <memory>

constexpr double kDefaultFrequency = 440.0;
constexpr int32_t kDefaultSampleRate = 48000;
constexpr double kPi = M_PI;
constexpr double kTwoPi = kPi * 2;

class Oscillator {

public:

    ~Oscillator() = default;

    void setWaveOn(bool isWaveOn) {
        mIsWaveOn.store(isWaveOn);
    };

    void setSampleRate(int32_t sampleRate) {
        mSampleRate = sampleRate;
        updatePhaseIncrement();
    };

    void setFrequency(double frequency) {
        mFrequency = frequency;
        updatePhaseIncrement();
    };

    inline void setAmplitude(float amplitude) {
        mAmplitude = amplitude;
    };

    void renderAudio(void *audioData, int32_t mono_frames, bool isStereo, int32_t channelCount) {

        if (mIsWaveOn){

            // temporary data stream
            float * mono_buffer = new float[mono_frames];

            for (int i = 0; i < mono_frames; ++i) {

                // Sine wave (sinf)
                //data[i*kChannelCount] = sinf(mPhase) * mAmplitude;

                // Square wave
                if (mPhase <= kPi){
                    mono_buffer[i] = -mAmplitude;
                } else {
                    mono_buffer[i] = mAmplitude;
                }

                mPhase += mPhaseIncrement;
                if (mPhase > kTwoPi) mPhase -= kTwoPi;
            }

            if (isStereo) {
                // the stereo buffer is twice as big as the mono buffer
                int32_t stereo_frames = mono_frames * 2;
                float *stereo_buffer = new float[stereo_frames];

                // stereo to mono
                for (size_t i = 0; i < mono_frames; ++i) {
                    stereo_buffer[i * 2] = mono_buffer[i];
                    stereo_buffer[i * 2 + 1] = mono_buffer[i];
                }

                memcpy(audioData, stereo_buffer, sizeof(float) * stereo_frames);

//                int16_t * ad = reinterpret_cast<int16_t *>(audioData);
//                for (int i = 0; i < mono_frames; ++i) {
//                    for (int j = 0; j < channelCount; ++j) {
//                        ad[(i * channelCount) + j] = stereo_buffer[(j * channelCount) + j];
//                    }
//                }

//                int16_t * ad = reinterpret_cast<int16_t *>(audioData);
//                for (int i = 0; i < stereo_frames; ++i) ad[i] = (int16_t) stereo_buffer[i];
//                for (int i = 0; i < stereo_frames; ++i) {
//
//                    // https://www.cs.cmu.edu/~rbd/papers/cmj-float-to-int.html
//
//                    // The natural way to implement the conversion is to scale each floating point
//                    // sample to some appropriate range (-32767 to 32767) and assign it to a
//                    // signed 16-bit integer as follows:
//
//                    float floatFrame = stereo_buffer[i]; /* assume -32768 <= f <= 32767 */
//                    int16_t intFrame; /* could also be “shortint” */
////                    intFrame = (((int16_t) (floatFrame + 32768.5)) - 32768);
//                    intFrame = (int16_t) floatFrame;
//                    reinterpret_cast<int16_t *>(audioData)[i] = intFrame;
//                }
                delete[] mono_buffer;
                delete[] stereo_buffer;
            } else {
                memcpy(audioData, mono_buffer, sizeof(float) * mono_frames);
                delete[] mono_buffer;
            }
        } else {
            memset(audioData, 0, sizeof(float) * mono_frames);
        }
    };

private:
    std::atomic<bool> mIsWaveOn { false };
    float mPhase = 0.0;
    std::atomic<float> mAmplitude { 0 };
    std::atomic<double> mPhaseIncrement { 0.0 };
    double mFrequency = kDefaultFrequency;
    int32_t mSampleRate = kDefaultSampleRate;

    void updatePhaseIncrement(){
        mPhaseIncrement.store((kTwoPi * mFrequency) / static_cast<double>(mSampleRate));
    };
};



#endif //LOWLATENCYAUDIO_OSCILLATOR_H
