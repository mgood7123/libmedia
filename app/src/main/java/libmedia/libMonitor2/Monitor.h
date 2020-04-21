//
// Created by konek on 7/1/2019.
//

#ifndef MEDIA_PLAYER_PRO_MONITOR_H
#define MEDIA_PLAYER_PRO_MONITOR_H


#include <android/log.h>
#include <OboeDebug.h>

#include <string>
#include <variant>
#include <deque>

#define MONITOR_TYPES std::variant<Monitor<bool*>*,Monitor<uint32_t*>*>

template <class TYPE> class Monitor {
public:
    bool anyChanged();

    bool changed(std::string IDENTIFIER);

    bool exists(std::string IDENTIFIER);

    int indexOf(std::string IDENTIFIER);

    void add(std::string IDENTIFIER, TYPE what);

    void remove(std::string IDENTIFIER);

private:
    class mon {
    public:
        std::string IDENTIFIER = "";
        TYPE current = 0;
        TYPE previous = 0;
    };
    std::deque<mon> a = std::deque<mon>();
};

#endif //MEDIA_PLAYER_PRO_MONITOR_H
