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

};


#endif //MEDIA_PLAYER_PRO_AUDIOTOOLS_H
