//
// Created by Mac on 16/4/20.
//

#include "Monitor.h"

template<class TYPE>
bool Monitor<TYPE>::anyChanged() {
    for (unsigned int i = 0; i < a.size(); i++) {
        TYPE c1 = static_cast<TYPE>(a[i].current);
        TYPE c2 = static_cast<TYPE>(a[i].previous);
        if (*c1 != *c2) {
            *static_cast<TYPE>(a[i].previous) = *static_cast<TYPE>(a[i].current);
            LOGE("VALUE CHANGED");
            return true;
        }
    }
    return false;
}

template<class TYPE>
bool Monitor<TYPE>::changed(std::string IDENTIFIER) {
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

template<class TYPE>
bool Monitor<TYPE>::exists(std::string IDENTIFIER) {
    for (unsigned int i = 0; i < a.size(); i++)
        if (a[i].IDENTIFIER == IDENTIFIER)
            return true;
    return false;
}

template<class TYPE>
int Monitor<TYPE>::indexOf(std::string IDENTIFIER) {
    for (unsigned int i = 0; i < a.size(); i++)
        if (a[i].IDENTIFIER == IDENTIFIER)
            return i;
    return -1;
}

template<class TYPE>
void Monitor<TYPE>::add(std::string IDENTIFIER, TYPE what) {
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

template<class TYPE>
void Monitor<TYPE>::remove(std::string IDENTIFIER) {
    // could use exists but then it would loop twice
    int index = indexOf(IDENTIFIER);
    if (index != -1) {
        return;
    } else {
        // rebuild the queue
        // TODO
        return;
        for (unsigned int i = 0; i < a.size(); i++) {
            TYPE current = a[i].current;
            // TODO: improve
            LOGE("REMOVING VALUE FROM MONITOR LIST");
            free(a[index].previous);
        }
    }
}