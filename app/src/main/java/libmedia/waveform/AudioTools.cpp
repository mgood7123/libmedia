//
// Created by konek on 6/23/2019.
//

#include "AudioTools.h"

void AudioTools::zero(int16_t *DATA, uint64_t samples) {
    for (int i = 0; i<=samples; i++) DATA[i] = 0;
}

void AudioTools::clone(int16_t *in, int16_t *out, uint64_t samples) {
    for (int i = 0; i<=samples; i++) out[i] = in[i];
}

void AudioTools::scale(int16_t *in, int16_t *out, uint64_t samples, int16_t height) {
    for (int i = 0; i<=samples; i++)
        out[i] = static_cast<int16_t>(((float)(in[i] + 32768) / 65536.0F) * (float)height);
}
