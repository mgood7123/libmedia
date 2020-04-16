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


#include "Generator.h"

void Generator::SetGeneratorSampleFrequency(int freq) {
    sampleFrequency = freq;
    phaseSumSine = (float)(2.0f * M_PI*waveFrequency)/sampleFrequency;
    samplesInHalfPeriode =  (int)(sampleFrequency/(2.0f*waveFrequency));
}

void Generator::SetGeneratorWaveFreqeuncy(float freq) {
    waveFrequency = freq;
    phaseSumSine = (float)(2.0f * M_PI*waveFrequency)/sampleFrequency;
    samplesInHalfPeriode =  (int)(sampleFrequency/(2.0f*waveFrequency));
}

void Generator::SetGeneratorWaveOn(bool on) {
    // isOn = on;
    isWaveOn_.store(on);
}

void Generator::Generate(float *audioData, int32_t numFrames) {
    switch (waveType) {
        case Square:
            GenerateSquareWave(audioData, numFrames);
            break;
        case Sawtooth:
            GenerateSawtoohWave(audioData, numFrames);
            break;
        case Triangular:
            GenerateTriangularWave(audioData, numFrames);
            break;
        default:
            GenerateSineWave(audioData, numFrames);
            break;
    }
}

void Generator::GenerateSineWave(float *audioData, int32_t numFrames){

    if (!isWaveOn_.load()){
        phase = 0;
    }
    for (int i = 0; i < numFrames; i++) {
        //if(isOn) {
        if (isWaveOn_.load()) {
            audioData[i] = (float) sin(phase)*volume;
            phase += phaseSumSine;
            if (phase > 2 * M_PI){
                phase -= 2 * M_PI;
            }
        }else {
            audioData[i] = 0;
        }
    }
}

void Generator::GenerateSquareWave(float *audioData, int32_t numFrames) {

    if (!isWaveOn_.load()){
        phase = 0;
        j=0;
    }
    for (int i = 0; i < numFrames; i++) {
        //if(isOn) {
        if (isWaveOn_.load()) {
            if(j<=samplesInHalfPeriode) {
                audioData[i] = volume;
            }else {
                audioData[i] = 0;
            }
            j++;
            if(j>=2*samplesInHalfPeriode) {
                j=0;
            }
        }else {
            audioData[i] = 0;
        }
    }
}

void Generator::GenerateSawtoohWave(float *audioData, int32_t numFrames) {

    if (!isWaveOn_.load()){
        phase = 0;
        j=0;
    }
    for (int i = 0; i < numFrames; i++) {
        //if(isOn) {
        if (isWaveOn_.load()) {
            audioData[i] = (volume/(2.0f*samplesInHalfPeriode))*j;
            j++;
            if(j>=2*samplesInHalfPeriode) {
                __android_log_print(ANDROID_LOG_ERROR, "GenerateSawtoohWave", "j %d",
                                    j);
                j=0;
            }
        }else {
            audioData[i] = 0;
        }
    }
}

void Generator::GenerateTriangularWave(float *audioData, int32_t numFrames) {

    if (!isWaveOn_.load()){
        phase = 0;
        j=0;
    }
    for (int i = 0; i < numFrames; i++) {
        //if(isOn) {
        if (isWaveOn_.load()) {
            if(j<=samplesInHalfPeriode) {
                audioData[i] = (volume/((float)samplesInHalfPeriode))*j;
            }else {
                audioData[i] = volume-((volume/((float)samplesInHalfPeriode))*(j-samplesInHalfPeriode));
            }
            j++;
            if(j>=2*samplesInHalfPeriode) {
                j=0;
            }
        }else {
            audioData[i] = 0;
        }
    }
}


void Generator::SetGeneratorWaveVolume(float vol) {
    volume = vol;
}

void Generator::SetGeneratorWaveType(int waveType_) {
    waveType = (WaveType)waveType_;
}
