//
// Created by konek on 7/19/2019.
//

#include "WindowsAPIKernel.h"

Kernel::Kernel() {
    this->table = new WindowsAPITable();
}

Kernel::~Kernel() {
    delete this->table;
}

Kernel KERNEL = Kernel();