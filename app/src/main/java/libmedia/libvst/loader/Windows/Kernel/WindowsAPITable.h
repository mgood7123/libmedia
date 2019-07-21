//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPITABLE_H
#define MEDIA_PLAYER_PRO_WINDOWSAPITABLE_H

#include <vector>
#include "../WindowsAPIDefinitions.h"
#include "WindowsAPIObject.h"

typedef class Table {
public:
    ~Table() {
        this->Page.removeAll();
    }
    std::vector<Object*> table;
    const size_t page_size = 1_kilobyte;
    Table();
    bool hasFreeIndex();
    size_t nextFreeIndex();
    bool hasObject(Object * object);
    size_t findObject(Object * object);
    Object * add(ObjectType type, DWORD flags);
    Object * add(ObjectType type, DWORD flags, PVOID resource);
    Object * add(Object * object);
    Object * add(Object & object);
    void DELETE(size_t index);
    void remove(Object * object);
    void remove(Object & object);

    class Page {
    public:
        Table * table;
        void add();
        void remove();
        void removeAll();
        void allocate(size_t size);
        int count();
        int indexToPageIndex(size_t index);
        void clean(int page);
        void cleanAll();
        void zero(int page);
        void zeroAll();
    } Page;

} WindowsAPITable;
#endif //MEDIA_PLAYER_PRO_WINDOWSAPITABLE_H
