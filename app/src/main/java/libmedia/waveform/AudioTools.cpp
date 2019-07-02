//
// Created by konek on 6/23/2019.
//

#include "AudioTools.h"

void AudioTools::zero(int16_t *DATA, uint64_t inSamples) {
    for (uint64_t i = 0; i<=inSamples; i++) DATA[i] = 0;
}

void AudioTools::clone(int16_t *in, int16_t *out, uint64_t inSamples) {
    for (uint64_t i = 0; i<=inSamples; i++) out[i] = in[i];
}

void AudioTools::splitStereo(int16_t *in, int16_t *outLeft, int16_t *outRight, uint64_t inSamples) {
    for (uint64_t i = 0; i<=inSamples/2; i++) {
        uint64_t leftout = i;
        uint64_t rightout = leftout;
        uint64_t leftin = leftout*2;
        uint64_t rightin = leftin+1;
        i++;
        outLeft[leftout] = in[leftin];
        outRight[rightout] = in[rightin];
    }
}

void AudioTools::scale(int16_t *in, int16_t *out, uint64_t inSamples, int16_t height) {
    for (uint64_t i = 0; i<=inSamples; i++)
        out[i] = static_cast<int16_t>(((float)(in[i] + 32768) / 65536.0F) * (float)height);
}

void AudioTools::crop(int16_t *in, uint32_t cropval, int16_t *out, uint64_t inSamples) {
    for (uint64_t i = 0; i<=inSamples; i++)
        if (in[i] < cropval) out[i] = in[i];
        else out[i] = 0;
}

void AudioTools::combineStereo(int16_t *out, int16_t *inLeft, int16_t *inRight, uint64_t inSamples) {
    // in left: 1 2 3 4
    // in right: 5 6 7 8
    // out: 1 5 2 6 3 7 4 8
    for (uint64_t i = 0; i<=inSamples*2; i++) {
        uint64_t leftout = i*2;
        uint64_t rightout = leftout+1;
        uint64_t leftin = i;
        uint64_t rightin = leftin;
        i++;
        out[leftout] = inLeft[leftin];
        out[rightout] = inRight[rightin];
    }
}
