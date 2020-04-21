//
// Created by konek on 6/23/2019.
//

#ifndef MEDIA_PLAYER_PRO_INTERNAL_LOOPER_H
#define MEDIA_PLAYER_PRO_INTERNAL_LOOPER_H

class Looper {
public:
    bool looperEnabled = false;
    AudioTime_Format start = 0;
    AudioTime_Format end = 0;
    int type = 0;
};

#endif //MEDIA_PLAYER_PRO_INTERNAL_LOOPER_H
