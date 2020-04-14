/*
 * Copyright 2018 Deniz A. Atlihan
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

#include <cstdint>
#include <math.h>
#include <android/log.h>
#include <atomic>

enum WaveType{
    Sine,
    Square,
    Sawtooth,
    Triangular
};

class Generator {

public:
    void Generate(float *audioData, int32_t numFrames);
    void SetGeneratorWaveVolume(float vol);
    void SetGeneratorWaveFreqeuncy(float freq);
    void SetGeneratorSampleFrequency(int freq);
    void SetGeneratorWaveOn(bool on);
    void SetGeneratorWaveType(int waveType_);

private:
    float volume = 0.5f;
    float waveFrequency = 880.0f;
    int sampleFrequency = 48000;

    float phase = 0;
    float phaseSumSine = (2 * M_PI*880)/48000.0f;
    int samplesInHalfPeriode = (int)(48000/1760.0f);

    //bool isOn;
    std::atomic<bool> isWaveOn_{false};
    WaveType waveType = Sine;
    void GenerateSineWave(float *audioData, int32_t numFrames);
    void GenerateSquareWave(float *audioData, int32_t numFrames);
    void GenerateSawtoohWave(float *audioData, int32_t numFrames);
    void GenerateTriangularWave(float *audioData, int32_t numFrames);
    int j =0;
};
