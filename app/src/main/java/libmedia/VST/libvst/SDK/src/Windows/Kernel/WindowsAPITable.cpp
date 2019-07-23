//
// Created by konek on 7/18/2019.
//

#include <Windows/Kernel/WindowsAPITable.h>

Table::Table() {
    this->Page.table = this;
    assert(this->table.size() == 0 && this->Page.count() == 0);
}

bool Table::hasFreeIndex() {
    int page = 1;
    size_t index = 0;
    size_t page_size = this->page_size;
    for (; page <= this->Page.count(); page++) {
        index = ((page_size * page) - page_size);
        for (; index < page_size * page; index++)
            if (this->table[index] == nullptr)
                return true;
    }
    return false;
}

size_t Table::nextFreeIndex() {
    int page = 1;
    size_t index = 0;
    size_t page_size = this->page_size;
    for (; page <= this->Page.count(); page++) {
        index = ((page_size * page) - page_size);
        for (; index < page_size * page; index++)
            if (this->table[index] == nullptr)
                return index;
    }
    return 0;
}

bool Table::hasObject(Object *object) {
    int page = 1;
    size_t index = 0;
    size_t page_size = this->page_size;
    for (; page <= this->Page.count(); page++) {
        index = ((page_size * page) - page_size);
        for (; index < page_size * page; index++)
            if (this->table[index] != nullptr && object != nullptr)
                if (*this->table[index] == *object) return true;
    }
    return false;
}

size_t Table::findObject(Object *object) {
    int page = 1;
    size_t index = 0;
    size_t page_size = this->page_size;
    for (; page <= this->Page.count(); page++) {
        index = ((page_size * page) - page_size);
        for (; index < page_size * page; index++)
            if (this->table[index] != nullptr && object != nullptr)
                if (*this->table[index] == *object) return index;
    }
    return 0;
}

Object *Table::add(ObjectType type, DWORD flags) {
    return this->add(type, flags, nullptr);
}

Object *Table::add(ObjectType type, DWORD flags, PVOID resource) {
    if (this->Page.count() == 0 || !this->hasFreeIndex()) this->Page.add();
    size_t i = this->nextFreeIndex();
    this->table[i] = new Object();
    this->table[i]->type = type;
    this->table[i]->flags = flags;
    this->table[i]->resource = resource;
    return this->table[i];
}

Object *Table::add(Object *object) {
    if (object != nullptr) return this->add(*object);
    return nullptr;
}

Object *Table::add(Object & object) {
    if (!this->hasFreeIndex()) this->Page.add();
    size_t i = this->nextFreeIndex();
    this->table[i] = new Object();
    this->table[i]->inherit(object);
    return this->table[i];
}

void Table::DELETE(size_t index) {
    if (this->table[index] != nullptr) {
        this->table[index]->clean();
        delete this->table[index];
        this->table[index] = nullptr;
    }
}

void Table::remove(Object *object) {
    if (this->hasObject(object)) this->DELETE(this->findObject(object));
}

void Table::remove(Object & object) {
    this->remove(&object);
}

void Table::Page::add() {
    this->table->table.resize(this->table->table.size() + this->table->page_size);
    this->zero(this->indexToPageIndex(this->table->table.size()));
}

void Table::Page::remove() {
    if (count() != 0) {
        this->clean(this->indexToPageIndex(this->table->table.size()));
        this->table->table.resize(this->table->table.size() - this->table->page_size);
    }
}

void Table::Page::removeAll() {
    while(count() > 0) this->remove();
}

void Table::Page::allocate(size_t size) {
    int pn = this->count();
    int pr = this->indexToPageIndex(size);
    if (pn == pr) return;
    if (pn > pr) for (int x = pr; x < pn; x++) this->clean(x);
    this->table->table.resize(roundUp<size_t>(size, this->table->page_size));
    if (pn < pr) for (int x = pn; x < pr; x++) this->zero(x);
}

int Table::Page::count() {
    if (this->table->table.size() == 0) return 0;
    return this->indexToPageIndex(this->table->table.size());
}

int Table::Page::indexToPageIndex(size_t index) {
    return static_cast<int>(roundDown<size_t>(index, this->table->page_size) / this->table->page_size);
}

void Table::Page::clean(int page) {
    size_t index = 0;
    size_t page_size = this->table->page_size;
    index = ((page_size * page) - page_size);
    for (; index < page_size * page; index++)
        this->table->DELETE(index);
}

void Table::Page::cleanAll() {
    for (int page = 1; page <= count(); page++)
        this->clean(page);
}

void Table::Page::zero(int page) {
    size_t index = 0;
    size_t page_size = this->table->page_size;
    index = ((page_size * page) - page_size);
    for (; index < page_size * page; index++)
        this->table->table[index] = nullptr;
}

void Table::Page::zeroAll() {
    for (int page = 1; page <= count(); page++)
        this->zero(page);
}
