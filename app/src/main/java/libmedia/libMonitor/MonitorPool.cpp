//
// Created by konek on 7/1/2019.
//

#include "MonitorPool.h"

bool MonitorPool::changed() {
    bool changed = false;
    for (unsigned int i = 0; i < a.size(); i++) {
        if (c(i)) {
            changed = true;
        }
    }
    return changed;
}

bool MonitorPool::exists(std::string IDENTIFIER) {
    for (unsigned int i = 0; i < a.size(); i++)
        if (a[i].IDENTIFIER == IDENTIFIER)
            return true;
    return false;
}

int MonitorPool::indexOf(std::string IDENTIFIER) {
    for (unsigned int i = 0; i < a.size(); i++)
        if (a[i].IDENTIFIER == IDENTIFIER)
            return i;
    return -1;
}

void MonitorPool::add(std::string IDENTIFIER, MONITOR_TYPES what) {
    // could use exists but then it would loop twice
    int index = indexOf(IDENTIFIER);
    if (index != -1) {
        a[index].m = what;
    } else {
        LOGE("ADDING VALUE TO MONITOR LIST");
        mon m = mon();
        m.IDENTIFIER = IDENTIFIER;
        m.m = what;
        a.push_front(m);
    }
}

bool MonitorPool::c(int index) {
    return std::visit([](MONITOR_TYPES &&arg) {
        if (std::holds_alternative<Monitor<uint32_t*>*>(arg))
            return std::get<Monitor<uint32_t *>*>(arg)->changed();
        else if (std::holds_alternative<Monitor<bool*>*>(arg))
            return std::get<Monitor<bool *>*>(arg)->changed();
        else return false;
    }, a[index].m);
}
