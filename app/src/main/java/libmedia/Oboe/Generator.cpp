//
// Created by macropreprocessor on 17/06/19.
//

#include <include/oboe/Oboe.h>
#include <src/common/OboeDebug.h>
#include "Generator.h"

void Generator::Static(void * audioData, int32_t numFrames) {
    // We requested AudioFormat::Float so we assume we got it. For production code always check what format
    // the stream has and cast to the appropriate type.
    auto *outputData = static_cast<float *>(audioData);
    const float amplitude = 0.2f;
    LOGW("AudioEngine::onAudioReady: FILLING outputData with static for %d frames", numFrames);
    for (int i = 0; i < numFrames; ++i){
        outputData[i] = ((float)drand48() - 0.5f) * 2 * amplitude;
    }
    LOGW("AudioEngine::onAudioReady: FILLED");
}
