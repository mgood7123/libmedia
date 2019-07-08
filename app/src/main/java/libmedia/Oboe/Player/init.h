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
    streamBuilder.setCallback(&AudioEngine);
    GlobalTime.setCallback(env, type, &CALLBACK);
    oboe::DefaultStreamValues::SampleRate = (int32_t) sampleRate;
    oboe::DefaultStreamValues::FramesPerBurst = (int32_t) framesPerBurst;
    streamBuilder.setDirection(oboe::Direction::Output);
    // PerformanceMode MUST not be set to LowLatency in order to interact with external processors such as ViPER4Android
    // however Oboe currently crashes if it is not set to LowLatency
    streamBuilder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    streamBuilder.setSharingMode(oboe::SharingMode::Shared);
    streamBuilder.setFormat(oboe::AudioFormat::I16);
    streamBuilder.setChannelCount(oboe::ChannelCount::Stereo);
    streamBuilder.setSampleRate(sampleRate);
    oboe::Result result = streamBuilder.openStream(&stream);
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Init: Failed to create AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    LOGW("Oboe_Init: setting AudioStream buffer size");
    // Reduce stream latency by setting the buffer size to a multiple of the burst size
    oboe::Result result1 = stream->setBufferSizeInFrames(stream->getFramesPerBurst() * 2);
    if (result1 != oboe::Result::OK) {
        LOGE("Oboe_Init: Failed to set AudioStream buffer size. Error: %s", oboe::convertToText(result1));
        return;
    }
    LOGW("Oboe_Init: aquiring AudioStream format");
    oboe::AudioFormat format = stream->getFormat();
    LOGI("Oboe_Init: AudioStream format is %s", oboe::convertToText(format));
}

NATIVE(void, Oboe, Cleanup)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: closing AudioStream");
    oboe::Result result = stream->close();
    currentAudioTrack = {nullptr};
    LOGW("Oboe_Init: closed AudioStream");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to close AudioStream . Error: %s", oboe::convertToText(result));
    }
}


#endif //MEDIA_PLAYER_PRO_INIT_H
