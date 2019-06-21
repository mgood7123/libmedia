//
// Created by macropreprocessor on 16/06/19.
//

#include <include/oboe/Oboe.h>
#include <src/common/OboeDebug.h>
#include <native.h>
#include "AudioEngine.h"
#include <android/asset_manager_jni.h>


oboe::AudioStream *stream {nullptr};
oboe::AudioStreamBuilder streamBuilder;
AudioEngine AudioEngine;
AAssetManager *mAssetManager {nullptr};
SoundRecording *currentAudioTrack {nullptr};

class Looper {
public:
    bool looperEnabled = false;
    double start = 0;
    double end = 0;
    int type = 0;
};

Looper Looper;

class CALLBACK : public AudioTime::Callback {
    void nanosecond(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().nanoseconds) {
            if (currentTime->nanoseconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().nanoseconds, currentAudioTrack->AudioData));
            if (currentTime->nanoseconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
    void microsecond(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().microseconds) {
            if (currentTime->microseconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().microseconds, currentAudioTrack->AudioData));
            if (currentTime->microseconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
    void millisecond(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().milliseconds) {
            if (currentTime->milliseconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().milliseconds, currentAudioTrack->AudioData));
            if (currentTime->milliseconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
    void second(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().seconds) {
            if (currentTime->seconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().seconds, currentAudioTrack->AudioData));
            if (currentTime->seconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
    void minute(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().minutes) {
            if (currentTime->minutes < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().minutes, currentAudioTrack->AudioData));
            if (currentTime->minutes == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
    void hour(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().hours) {
            if (currentTime->hours < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().hours, currentAudioTrack->AudioData));
            if (currentTime->hours == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
};

CALLBACK CALLBACK;

class TIMECALLBACK : public AudioTime::Callback {
    int c = 0;
    void millisecondTenth(AudioTime * currentTime) {
        if ((currentTime->milliseconds == 250 && c >= 6) || (currentTime->milliseconds == 500 && c < 6)) {
            currentAudioTrack->resetPlayHead();
            c++;
            if (c == 10) c = 0;
        }
    }
};

TIMECALLBACK TIMECALLBACK;

Mixer Mixer;

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
    streamBuilder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    streamBuilder.setSharingMode(oboe::SharingMode::Exclusive);
    streamBuilder.setFormat(oboe::AudioFormat::I16);
    streamBuilder.setChannelCount(oboe::ChannelCount::Stereo);
    streamBuilder.setSampleRate(48000);
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

NATIVE(void, Oboe, LoadTrackFromAssets)(JNIEnv *env, jobject type, jobject jAssetManager) {
    mAssetManager = AAssetManager_fromJava(env, jAssetManager);
    currentAudioTrack = SoundRecording::loadFromAssets(mAssetManager, "CLAP.raw", streamBuilder.getSampleRate(), streamBuilder.getChannelCount());
    Mixer.addTrack(currentAudioTrack);
}

NATIVE(void, Oboe, Play)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: requesting Start");
    oboe::Result result = stream->requestStart();
    LOGW("Oboe_Init: requested Start");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to start AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    currentAudioTrack->setPlaying(true);
}

NATIVE(void, Oboe, Pause)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: requesting Pause");
    oboe::Result result = stream->requestPause();
    LOGW("Oboe_Init: requested Pause");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to pause AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    currentAudioTrack->setPlaying(false);
}

NATIVE(void, Oboe, Stop)(JNIEnv *env, jobject type) {
    LOGW("Oboe_Init: requesting Stop");
    oboe::Result result = stream->requestStop();
    LOGW("Oboe_Init: requested Stop");
    if (result != oboe::Result::OK) {
        LOGE("Oboe_Play: Failed to stop AudioStream . Error: %s", oboe::convertToText(result));
        return;
    }
    currentAudioTrack->setPlaying(true);
    currentAudioTrack->resetPlayHead();
}

NATIVE(void, Oboe, Loop)(JNIEnv *env, jobject type, jboolean value) {
    currentAudioTrack->setLooping(value);
}

NATIVE(void, Oboe, Looper)(JNIEnv *env, jobject type, jdouble start, jdouble end, jint timing) {
    Looper.type = timing;
    Looper.start = start;
    Looper.end = end;
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
