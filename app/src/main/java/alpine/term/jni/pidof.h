//
// Created by Mac on 12/5/20.
//

#ifndef ALPINE_TERM_APP_PIDOF_H
#define ALPINE_TERM_APP_PIDOF_H

#include <stdbool.h>
#include <sys/types.h>

extern bool pidof(const char *name, int * pid_count);
extern bool pidof_with_pid_list(const char *name, pid_t ** pid_list, int * pid_count);

#endif //ALPINE_TERM_APP_PIDOF_H
