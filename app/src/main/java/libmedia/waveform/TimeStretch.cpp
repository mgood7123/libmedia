//
// Created by konek on 6/23/2019.
//

#include <stdexcept>
#include "TimeStretch.h"
#include "AudioInformation.h"
void TimeStretch::Shorten::test(int16_t in[], uint64_t inSamples, int16_t out[], uint64_t outSamples) {
    // crude implementation
    if (outSamples > inSamples) return;
    uint64_t offset = (inSamples / outSamples);
    for (uint64_t i = 0; i < outSamples; i++) {
        out[i] = in[i*offset];
    }
}

uint64_t TimeStretch::Shorten::FrameToStretchedFrame(uint64_t inSamples, uint64_t outSamples, uint64_t wanted) {
    if (wanted == 0) return 0;
    uint64_t length = (inSamples / (inSamples / outSamples));
    if (wanted == inSamples) return length;
    uint64_t offset = (inSamples / outSamples);
    for (uint64_t i = 0; i < length; i++) {
        if (i * offset >= wanted) {
            return i; /* unstretched to stretched */
        }
    }
    // if nothing matches, return the end of the stretched sample
    return length;
}