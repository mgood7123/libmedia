//
// Created by konek on 7/1/2019.
//

#ifndef MEDIA_PLAYER_PRO_MONITORPOOL_H
#define MEDIA_PLAYER_PRO_MONITORPOOL_H

#include "Monitor.h"

class MonitorPool {
public:

    bool changed();

    bool exists(std::string IDENTIFIER);

    int indexOf(std::string IDENTIFIER);

    void add(std::string IDENTIFIER, MONITOR_TYPES what);

private:
    class mon {
    public:
        std::string IDENTIFIER = "";
        MONITOR_TYPES m;
    };

    std::deque<mon> a = std::deque<mon>();
    bool c(int index);
};


#endif //MEDIA_PLAYER_PRO_MONITORPOOL_H
