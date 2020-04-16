//
// Created by konek on 6/24/2019.
//

#ifndef MEDIA_PLAYER_PRO_CURRENTFRAME_H
#define MEDIA_PLAYER_PRO_CURRENTFRAME_H

#include "../../waveform/TimeStretch.h"

NATIVE(jint , Oboe, CurrentFrame) (JNIEnv *env, jobject thiz, jint width) {
    if (currentAudioTrack == NULL)
        return 0;
    else return static_cast<jint>(
        // GET CURRENT FRAME FROM THE WAVEFORM
        TimeStretch::Shorten::FrameToStretchedFrame(
            currentAudioTrack->AudioData->totalFrames, width, GlobalTime.currentFrame
        )
    );
}

#endif //MEDIA_PLAYER_PRO_CURRENTFRAME_H
