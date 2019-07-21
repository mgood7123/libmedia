//
// Created by konek on 7/18/2019.
//

#include "WindowsAPIObject.h"
#include "WindowsAPITable.h"
#include "WindowsAPIKernel.h"

extern const ObjectType ObjectTypeNone = 0;
extern const ObjectType ObjectTypeProcess = 1;
extern const ObjectType ObjectTypeThread = 2;
extern const ObjectType ObjectTypeWindow = 3;

Object * Kernel::newObject(ObjectType type, DWORD flags) {
    return this->table->add(type, flags, nullptr);
}

Object * Kernel::newObject(ObjectType type, DWORD flags, PVOID resource) {
    return this->table->add(type, flags, resource);
}

void Kernel::deleteObject(Object * object) {
    this->table->remove(object);
}

void Kernel::deleteObject(Object & object) {
    this->table->remove(object);
}

Object::Object() {
    this->init();
}

Object::~Object() {
    this->clean();
}

void Object::clean() {
    this->clean(*this);
}

void Object::clean(Object & object) {
    if (object.name != nullptr) {
        memset(object.name, '\0', strlen(object.name));
        this->init(object);
    }
}

void Object::init() {
    this->init(*this);
}

void Object::init(Object & object) {
    object.name = nullptr;
    object.type = ObjectTypeNone;
    object.flags = 0;
    object.handles = 0;
    object.resource = nullptr;
}

Object &Object::operator=(const Object &object) {
    this->inherit(object);
    return *this;
}

void Object::inherit(const Object & object) {
    if (object.name == nullptr) this->name = nullptr;
    else strncpy(this->name, object.name, strlen(object.name));
    this->flags = object.flags;
    this->type = object.type;
    this->resource = object.resource;
}

bool Object::compare(const Object &lhs, const Object &rhs) {
    if (lhs.type != rhs.type) return false;
    if (lhs.handles != rhs.handles) return false;
    if (lhs.flags != rhs.flags) return false;
    if (lhs.name == nullptr && rhs.name == nullptr) {}
    if (lhs.name != nullptr && rhs.name != nullptr) {
        if (strcmp(lhs.name, rhs.name) != 0) return false;
    }
    return lhs.resource == rhs.resource;
}
