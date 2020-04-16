//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_PLAYBACK_H
#define MEDIA_PLAYER_PRO_PLAYBACK_H

#include "core.h"

bool STREAM_STARTED = false; // we only use a single stream for now

/*
 * IMPORTANT: avoid starting and stopping the `oboe::AudioStream *stream` rapidly
 * exact reason appears to due to a bug in the AAudio Legacy path for Android P (9),
 *
 * W/AudioStreamLegacy: processCallbackCommon() stopping because callback disabled
 * E/AudioTrack: EVENT_MORE_DATA requested 256 bytes but callback returned -1 bytes
 *
 * see https://github.com/google/oboe/issues/396 :
 *
 * When I returned oboe::DataCallbackResult::Stop in the input stream callback, the callback stopped
 * being called as expected, but the stream state remained in Stopping and never changed to Stopped.
 *
 * In this case, the callback stopped, but the stream state remained in Started with Errors
 * W/AudioStreamLegacy: processCallbackCommon() callback requested stop, fake an error and
 * E/AudioRecord: EVENT_MORE_DATA requested 384 bytes but callback returned -1 bytes
 *
 * And when I tried to restart the stream by calling requestStart(), it didn't work.
 *
 * That is due to a bug in the AAudio Legacy path for Android P (9).
 *
 * This and other bugs related to DataCallbackResult::Stop have been fixed for a future Android
 * release.
*/

// TODO: guard against playing without first loading an audio track
//  in multi track situations such as when using the Mixer, simply relying on currentAudioTrack is
//  unreliable

bool Oboe_Stream_Start() {
    LOGW("Oboe_Init: requesting Start");
    AudioEngine.StartStream();
    LOGW("Oboe_Init: requested Start");
    STREAM_STARTED = true;
    return true;
}

bool Oboe_Stream_Stop() {
    LOGW("Oboe_Init: requesting Stop");
    AudioEngine.StopStream();
    LOGW("Oboe_Init: requested Stop");
    STREAM_STARTED = false;
    return true;
}

NATIVE(void, Oboe, Play)(JNIEnv *env, jobject type) {
    if (currentAudioTrack != NULL) {
        if (!STREAM_STARTED) Oboe_Stream_Start();
        currentAudioTrack->setPlaying(true);
    }
}

NATIVE(void, Oboe, Pause)(JNIEnv *env, jobject type) {
    if (currentAudioTrack != NULL) {
        currentAudioTrack->setPlaying(false);
    }
}

NATIVE(void, Oboe, Stop)(JNIEnv *env, jobject type) {
    if (currentAudioTrack != NULL) {
        if (STREAM_STARTED) Oboe_Stream_Stop();
        currentAudioTrack->setPlaying(false);
        currentAudioTrack->resetPlayHead();
    }
}

#endif //MEDIA_PLAYER_PRO_PLAYBACK_H
