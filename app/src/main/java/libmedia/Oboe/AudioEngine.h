//
// Created by macropreprocessor on 17/06/19.
//

#ifndef MEDIA_PLAYER_PRO_AUDIOENGINE_H
#define MEDIA_PLAYER_PRO_AUDIOENGINE_H
#include <AudioTime.h>
#include <aaudio/AAudio.h>
#include "Generator.h"

extern AudioTime GlobalTime;

class AudioEngine {
public:
    AAudioStreamBuilder *builder;
    AAudioStream *stream;
    int sampleRate = 48000;
    int BufferCapacityInFrames = 192;
    int channelCount = 2;
    Generator generator;

    static aaudio_data_callback_result_t onAudioReady(
            AAudioStream *stream, void *userData, void *audioData, int32_t numFrames
    );

    static void onError(
            AAudioStream *stream, void *userData, aaudio_result_t error
    );

    void RestartStream();

    aaudio_result_t CreateAndOpenStream(
            int sample_rate, int Buffer_Capacity_In_Frames, int channel_count
    );

    aaudio_result_t StartStream();

    aaudio_result_t PauseStream();

    aaudio_result_t StopStream();

    aaudio_result_t FlushStream();

    aaudio_result_t ChangeState(aaudio_stream_state_t inputState, aaudio_stream_state_t nextState);

    aaudio_result_t CreateAndOpenStream();
};

#include "audio/Mixer.h"

#endif //MEDIA_PLAYER_PRO_AUDIOENGINE_H
