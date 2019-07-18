//
// Created by konek on 7/18/2019.
//

#include "WindowsAPIObject.h"
#include "WindowsAPITable.h"
#include "WindowsAPIKernel.h"

Object * Kernel::newObject(ObjectType type, DWORD flags) {
    if (KERNEL.table == nullptr) KERNEL.table = new WindowsAPITable();
    return KERNEL.table->add(type, flags);
};

void Kernel::deleteObject(Object * object) {
    KERNEL.table->remove(object);
}

void Kernel::deleteObject(Object object) {
    KERNEL.table->remove(&object);
}