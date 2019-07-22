//
// Created by konek on 7/22/2019.
//

#include <Windows/Locker.h>

Locker::Locker() {
    this->LOCK = false;
}

void Locker::lock() {
    if (this->LOCK) while (this->LOCK) {}
    else this->LOCK = true;
}
void Locker::unlock() {
    this->LOCK = false;
}
