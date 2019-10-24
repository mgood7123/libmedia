//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_AUDIOTOOLS_H
#define MEDIA_PLAYER_PRO_AUDIOTOOLS_H

#include <cstdint>

class AudioTools {
public:
    static void zero(int16_t DATA[], uint64_t samples);
    static void clone(int16_t in[], int16_t out[], uint64_t samples);
    static void scale(int16_t in[], int16_t out[], uint64_t samples, int16_t height);
    static void splitStereo(int16_t *in, int16_t *outLeft, int16_t *outRight, uint64_t inSamples);
    static void crop(int16_t *in, uint32_t cropval, int16_t *out, uint64_t inSamples);
    static void combineStereo(int16_t *out, int16_t *inLeft, int16_t *inRight, uint64_t inSamples);
    template <typename inType, typename outType>
    static void convert(inType *in, outType *out, uint64_t inSamples) {
        for (uint64_t i = 0; i<=inSamples; i++) out[i] = static_cast<outType>(in[i]);
    }
};


#endif //MEDIA_PLAYER_PRO_AUDIOTOOLS_H
