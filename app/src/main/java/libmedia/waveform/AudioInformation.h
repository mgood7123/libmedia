//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_AUDIOINFORMATION_H
#define MEDIA_PLAYER_PRO_AUDIOINFORMATION_H

class AudioInformation {
    class Bits {
        int _16 = 1;
        int _24 = 2;
        int _32 = 3;
    };
    class Rate {
        int _8k = 1;
        int _16k = 2;
        int _24k = 3;
        int _44k = 4;
        int _48k = 5;
        int _96k = 6;
    };
};

#endif //MEDIA_PLAYER_PRO_AUDIOINFORMATION_H
