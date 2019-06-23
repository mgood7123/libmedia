//
// Created by konek on 6/23/2019.
//

#include "TimeStretch.h"
#include "AudioInformation.h"
void TimeStretch::Shorten::test(int16_t in[], uint64_t inSamples, int16_t out[], uint64_t outSamples) {
    // crude implementation
    if (outSamples > inSamples) return;
    uint64_t length = (inSamples / (inSamples / outSamples));
    uint64_t offset = (inSamples / outSamples);
    for (uint64_t i = 0; i < length; i++) {
        int16_t data = in[i*offset];
        out[i] = data;
    }
}
