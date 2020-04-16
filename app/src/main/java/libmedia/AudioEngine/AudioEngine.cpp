//
// Created by macropreprocessor on 17/06/19.
//

#include <OboeDebug.h>
#include "AudioEngine.h"
#include "Player/Oscillator/Oscillator.h"
#include <AudioTime.h>

#include <thread>

extern Mixer Mixer;
extern AudioEngine AudioEngine;

aaudio_data_callback_result_t AudioEngine::onAudioReady(
        AAudioStream *stream, void *userData, void *audioData, int32_t numFrames
) {
    AudioEngine * AE = static_cast<AudioEngine *>(userData);
    Mixer.renderAudio(static_cast<int16_t *>(audioData), numFrames);

    // Are we getting underruns?
    int32_t tmpuc = AAudioStream_getXRunCount(stream);
    if (tmpuc > AE->previousUnderrunCount) {
        AE->previousUnderrunCount = AE->underrunCount;
        AE->underrunCount = tmpuc;
        // Try increasing the buffer size by one burst
        AE->bufferSize += AE->framesPerBurst;
        AE->bufferSize = AAudioStream_setBufferSizeInFrames(stream, AE->bufferSize);
    }
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void AudioEngine::onError(
        AAudioStream *stream, void *userData, aaudio_result_t error
){
    if (error == AAUDIO_ERROR_DISCONNECTED){
        std::function<void(void)> restartFunction = std::bind(&AudioEngine::RestartStream,
                                                              static_cast<AudioEngine *>(userData));
        new std::thread(restartFunction);
    }
}

void AudioEngine::RestartStream(){

    static std::mutex restartingLock;
    if (restartingLock.try_lock()){
        StopStream();
        StartStream();
        restartingLock.unlock();
    }
}

aaudio_result_t AudioEngine::CreateAndOpenStream(
        int sample_rate, int Buffer_Capacity_In_Frames, int channel_count
) {
    sampleRate = sample_rate;
    BufferCapacityInFrames = Buffer_Capacity_In_Frames;
    channelCount = channel_count;
    return CreateAndOpenStream();
}

aaudio_result_t AudioEngine::CreateAndOpenStream() {
    aaudio_result_t result = AAudio_createStreamBuilder(&builder);
    if (result != AAUDIO_OK) {
        return result;
    }

    AAudioStreamBuilder_setDeviceId(builder, 0);
    AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setSampleRate(builder, sampleRate);
    AAudioStreamBuilder_setBufferCapacityInFrames(builder, BufferCapacityInFrames*2);
    AAudioStreamBuilder_setChannelCount(builder, channelCount);
    AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setDataCallback(builder, onAudioReady, this);
    AAudioStreamBuilder_setErrorCallback(builder, onError, this);
    AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    result = AAudioStreamBuilder_openStream(builder, &stream);

    underrunCount = 0;
    previousUnderrunCount = 0;
    framesPerBurst = AAudioStream_getFramesPerBurst(stream);
    bufferSize = AAudioStream_getBufferSizeInFrames(stream);
    bufferCapacity = AAudioStream_getBufferCapacityInFrames(stream);
    return result;
}

aaudio_result_t AudioEngine::StartStream() {
    aaudio_result_t result =  AAudioStream_requestStart(stream);
    return result;
}

aaudio_result_t AudioEngine::PauseStream() {
    aaudio_result_t result = AAudioStream_requestPause(stream);
    return result;
}

aaudio_result_t AudioEngine::StopStream() {
    aaudio_result_t result = AAudioStream_requestStop(stream);
    return result;
}

aaudio_result_t AudioEngine::FlushStream() {
    aaudio_result_t result = AAudioStream_requestFlush(stream);
    AAudioStreamBuilder_delete(builder);
    return result;
}

aaudio_result_t
AudioEngine::ChangeState(aaudio_stream_state_t inputState, aaudio_stream_state_t nextState) {
    int64_t timeoutNanos = 100;
    aaudio_result_t result = AAudioStream_waitForStateChange(stream, inputState, &nextState, timeoutNanos);
    return result;
}
