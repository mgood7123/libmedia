//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_TIMESTRETCH_H
#define MEDIA_PLAYER_PRO_TIMESTRETCH_H

#include <cstdint>

class TimeStretch {
    class Lengthen{

    };

public:
    class Shorten {
    public:
        static void test(int16_t in[], uint64_t inSamples, int16_t out[], uint64_t outSamples);

        static uint64_t FrameToStretchedFrame(uint64_t inSamples, uint64_t outSamples, uint64_t wanted);
    };
};


#endif //MEDIA_PLAYER_PRO_TIMESTRETCH_H
