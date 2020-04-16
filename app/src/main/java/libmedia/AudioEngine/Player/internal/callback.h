//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_INTERNAL_CALLBACK_H
#define MEDIA_PLAYER_PRO_INTERNAL_CALLBACK_H

#include <AudioTime.h>

class CALLBACK : public AudioTime::Callback {
public:
    virtual ~CALLBACK() = default;
    virtual void nanosecond(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().nanoseconds) {
            if (currentTime->nanoseconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().nanoseconds, currentAudioTrack->AudioData));
            if (currentTime->nanoseconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }

    virtual void microsecond(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().microseconds) {
            if (currentTime->microseconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().microseconds, currentAudioTrack->AudioData));
            if (currentTime->microseconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }

    virtual void millisecond(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().milliseconds) {
            if (currentTime->milliseconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().milliseconds, currentAudioTrack->AudioData));
            if (currentTime->milliseconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }

    virtual void second(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().seconds) {
            if (currentTime->seconds < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().seconds, currentAudioTrack->AudioData));
            if (currentTime->seconds == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }

    virtual void minute(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().minutes) {
            if (currentTime->minutes < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().minutes, currentAudioTrack->AudioData));
            if (currentTime->minutes == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }

    virtual void hour(AudioTime *currentTime) {
        if (Looper.type == AudioTime::types().hours) {
            if (currentTime->hours < Looper.start) currentAudioTrack->seekTo(currentTime->toFrame(Looper.start, AudioTime::types().hours, currentAudioTrack->AudioData));
            if (currentTime->hours == Looper.end) currentAudioTrack->resetPlayHead();
        }
    }
};

#endif //MEDIA_PLAYER_PRO_INTERNAL_CALLBACK_H
