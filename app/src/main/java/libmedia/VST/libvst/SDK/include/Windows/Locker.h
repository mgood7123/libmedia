//
// Created by konek on 7/22/2019.
//

#ifndef MEDIA_PLAYER_PRO_LOCKER_H
#define MEDIA_PLAYER_PRO_LOCKER_H


typedef class Locker {
private:
    bool LOCK;
public:
    Locker();
    void lock();
    void unlock();
} Locker;


#endif //MEDIA_PLAYER_PRO_LOCKER_H
