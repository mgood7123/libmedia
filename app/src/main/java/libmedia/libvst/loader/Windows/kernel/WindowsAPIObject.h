//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H

#include "../WindowsAPIDefinitions.h"
#include <cstring>

typedef int ObjectType;
ObjectType ObjectTypeNone = 0;
ObjectType ObjectTypeProcess;
ObjectType ObjectTypeThread;
ObjectType ObjectTypeWindow;


typedef class Object {
public:
    Object() {
        init();
    }
    ~Object() {
        clean();
    }
    ObjectType type;
    char *name;
    DWORD flags;
    int handles;
    PVOID resource;

    void clean() {
        clean(*this);
    }
    void clean(Object object) {
        if (object.name != nullptr) memset(object.name, '\0', strlen(object.name));
        init(object);
    }
    void init() {
        init(*this);
    }
    void init(Object object) {
        object.name = nullptr;
        object.type = ObjectTypeNone;
        object.flags = 0;
        object.handles = 0;
        object.resource = nullptr;
    }
    Object & operator=(const Object& object)
    {
        inherit(object);
        return *this;
    }
    void inherit(Object object) {
        if (object.name == nullptr) name = nullptr;
        else strncpy(name, object.name, strlen(object.name));
        flags = object.flags;
        type = object.type;
        resource = object.resource;
    }
    inline bool operator==(const Object& lhs, const Object& rhs)
    {
        return compare(lhs, rhs);
    }
    inline bool operator!=(const Object& lhs, const Object& rhs)
    {
        return !compare(lhs, rhs);
    }
    inline bool compare (const Object& lhs, const Object& rhs)
    {
        if (lhs.type != rhs.type) return false;
        if (lhs.handles != rhs.handles) return false;
        if (lhs.flags != rhs.flags) return false;
        if (lhs.name == nullptr && rhs.name == nullptr) {}
        if (lhs.name != nullptr && rhs.name != nullptr) {
            if (strcmp(lhs.name, rhs.name) != 0) return false;
        }
        if (lhs.resource != rhs.resource) return false;
        return true;
    }

} Object;

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
