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
    bool changed() {
        bool changed = false;
        for (unsigned int i = 0; i < a.size(); i++) {
            TYPE c1 = static_cast<TYPE>(a[i].current);
            TYPE c2 = static_cast<TYPE>(a[i].previous);
            if (*c1 != *c2) {
                *static_cast<TYPE>(a[i].previous) = *static_cast<TYPE>(a[i].current);
                LOGE("VALUE CHANGED");
                changed = true;
            }
        }
        return changed;
    }

    bool changed(std::string IDENTIFIER) {
        unsigned int i = indexOf(IDENTIFIER);
        if (i == -1) return false;
        TYPE c1 = static_cast<TYPE>(a[i].current);
        TYPE c2 = static_cast<TYPE>(a[i].previous);
        if (*c1 != *c2) {
            *static_cast<TYPE>(a[i].previous) = *static_cast<TYPE>(a[i].current);
            LOGE("VALUE CHANGED");
            return true;
        }
        return false;
    }

    bool exists(std::string IDENTIFIER) {
        for (unsigned int i = 0; i < a.size(); i++)
            if (a[i].IDENTIFIER == IDENTIFIER)
                return true;
        return false;
    }

    int indexOf(std::string IDENTIFIER) {
        for (unsigned int i = 0; i < a.size(); i++)
            if (a[i].IDENTIFIER == IDENTIFIER)
                return i;
        return -1;
    }

    void add(std::string IDENTIFIER, TYPE what) {
        // could use exists but then it would loop twice
        int index = indexOf(IDENTIFIER);
        if (index != -1) {
            a[index].current = what;
        } else {
            LOGE("ADDING VALUE TO MONITOR LIST");
            mon m = mon();
            m.IDENTIFIER = IDENTIFIER;
            m.current = what;
            m.previous = static_cast<TYPE>(malloc(1*sizeof(TYPE)));
            a.push_front(m);
        }
    }

    void remove(std::string IDENTIFIER) {
        // could use exists but then it would loop twice
        int index = indexOf(IDENTIFIER);
        if (index != -1) {
            return;
        } else {
            // TODO: improve
            LOGE("REMOVING VALUE FROM MONITOR LIST");
            free(a[index].previous);
        }
    }

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
