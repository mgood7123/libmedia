//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
#define MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H

#include "../WindowsAPIDefinitions.h"
#include <cstring>

typedef int ObjectType;
extern const ObjectType ObjectTypeNone;
extern const ObjectType ObjectTypeProcess;
extern const ObjectType ObjectTypeThread;
extern const ObjectType ObjectTypeWindow;


typedef class Object {
public:
    Object();
    ~Object();
    ObjectType type;
    char *name;
    DWORD flags;
    int handles;
    PVOID resource;

    void clean();
    void clean(Object & object);
    void init();
    void init(Object & object);
    Object & operator=(const Object& object);
    void inherit(const Object & object);
    inline bool operator==(const Object &rhs) { return compare(*this, rhs); }
    inline bool operator!=(const Object &rhs) { return !compare(*this, rhs); }
    static bool compare (const Object& lhs, const Object& rhs);

} Object;

#endif //MEDIA_PLAYER_PRO_WINDOWSAPIOBJECT_H
