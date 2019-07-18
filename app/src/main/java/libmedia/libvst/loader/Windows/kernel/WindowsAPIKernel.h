//
// Created by konek on 7/19/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIKERNEL_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIKERNEL_H

#include "../WindowsAPIDefinitions.h"
#include "WindowsAPIHandle.h"
#include "WindowsAPIObject.h"
#include "WindowsAPITable.h"
#include <cassert>
#include <cstdint>

class Kernel {
public:
    WindowsAPITable * table = nullptr;
    bool validHandle(HANDLE hObject) {
        if (hObject == nullptr) return 0;
        Handle * h = getHandle(hObject);
        if (h->object == nullptr) return 0;
        if (h->invalidated) return 0;
        return 1;
    }

    HANDLE newHandle(ObjectType type) {
        return newHandle(type, nullptr);
    }
    HANDLE newHandle(ObjectType type, PVOID resource) {
        Handle * hObject = new Handle();
        hObject->object = newObject(type, 0);
        hObject->object->handles++;
        if (resource != nullptr) hObject->object->resource = resource;
        assert(validHandle(hObject));
        return hObject;
    }
    Handle * getHandle(HANDLE handle) {
        return static_cast<Handle*>(handle);
    }
    Object * newObject(ObjectType type, DWORD flags);
    void deleteObject(Object * object);
    void deleteObject(Object object);

    pid PID_LAST = 0;

} KERNEL;


#endif //MEDIA_PLAYER_PRO_WINDOWSAPIKERNEL_H
