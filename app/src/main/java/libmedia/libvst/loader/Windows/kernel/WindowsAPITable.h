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
    static std::vector<Object*> table;
    static size_t page_size = 1kilobyte;
    Table() {
        assert(table.size() == 0 && Page.count() == 0);
    }
    class Page {
    public:
        void add() {
            table.resize(table.size() + page_size);
            zero(indexToPageIndex(table.size()));
        }
        void remove() {
            if (count() != 0) {
                clean(indexToPageIndex(table.size()));
                table.resize(table.size() - page_size);
            }
        }
        void allocate(size_t size) {
            int pn = count();
            int pr = indexToPageIndex(size);
            if (pn == pr) return;
            if (pn > pr) for (int x = pr; x < pn; x++) clean(x);
            table.resize(roundUp<size_t>(size, page_size));
            if (pn < pr) for (int x = pn; x < pr; x++) zero(x);
        }
        int count() {
            if (table.size() == 0) return 0;
            return indexToPageIndex(table.size());
        }
        int indexToPageIndex(size_t index) {
            return static_cast<int>(roundDown<size_t>(index, page_size) / page_size);
        }
        void clean(int page) {
            for (size_t index = page_size*page; index < page_size*(page*2); index++) DELETE(index);
        }
        void cleanAll() {
            for (int page = 0; page < count(); page++) clean(page);
        }
        void zero(int page) {
            for (size_t index = page_size*page; index < page_size*(page*2); index++) table[index] = nullptr;
        }
        void zeroAll() {
            for (int page = 0; page < count(); page++) zero(page);
        }
    } Page;

    bool hasFreeIndex() {
        for (int page = 0; page < Page.count(); page++)
            for (size_t index = page_size*page; index < page_size*(page*2); index++)
                if (table[index] != nullptr) return true;
        return false;
    }
    size_t nextFreeIndex() {
        for (int page = 0; page < Page.count(); page++)
            for (size_t index = page_size*page; index < page_size*(page*2); index++)
                if (table[index] != nullptr) return (page_size*page)+index;
        return 0;
    }
    bool hasObject(Object * object) {
        for (int page = 0; page < Page.count(); page++)
            for (size_t index = page_size*page; index < page_size*(page*2); index++)
                if (table[index] != nullptr && object != nullptr)
                    if (*table[index] == *object) return true;
        return false;
    }
    size_t findObject(Object * object) {
        for (int page = 0; page < Page.count(); page++)
            for (size_t index = page_size*page; index < page_size*(page*2); index++)
                if (table[index] != nullptr && object != nullptr)
                    if (*table[index] == *object) return (page_size*page)+index;
        return 0;
    }

    Object * add(ObjectType type, DWORD flags) {
        if (Page.count() == 0 || !hasFreeIndex()) Page.add();
        size_t i = nextFreeIndex();
        table[i] = new Object();
        table[i]->type = type;
        table[i]->flags = flags;
        return table[i];
    }

    Object * add(Object * object) {
        if (object != nullptr) return add(*object);
        return nullptr;
    }

    Object * add(Object object) {
        if (!hasFreeIndex()) Page.add();
        size_t i = nextFreeIndex();
        table[i] = new Object();
        table[i]->inherit(object);
        return table[i];
    }

    void DELETE(size_t index) {
        if (table[index] != nullptr) {
            table[index]->clean();
            delete table[index];
            table[index] = nullptr;
        }
    }

    void remove(Object * object) {
        if (hasObject(object)) DELETE(findObject(object));
    }
} WindowsAPITable;
#endif //MEDIA_PLAYER_PRO_WINDOWSAPITABLE_H
