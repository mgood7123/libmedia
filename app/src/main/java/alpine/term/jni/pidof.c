//
// Created by Mac on 12/5/20.
//

#include "pidof.h"
#include <stdio.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

char libbuf[4096];

int get_block_device_size(int fd, unsigned long long* size)
{
    return (ioctl(fd, BLKGETSIZE64, size) >= 0);
}


// Return how long the file at fd is, if there's any way to determine it.
off_t fdlength(int fd)
{
    struct stat st;
    off_t base = 0, range = 1, expand = 1, old;
    unsigned long long size;

    if (!fstat(fd, &st) && S_ISREG(st.st_mode)) return st.st_size;

    // If the ioctl works for this, return it.
    if (get_block_device_size(fd, &size)) return size;

    // If not, do a binary search for the last location we can read.  (Some
    // block devices don't do BLKGETSIZE right.)  This should probably have
    // a CONFIG option...
    old = lseek(fd, 0, SEEK_CUR);
    do {
        char temp;
        off_t pos = base + range / 2;

        if (lseek(fd, pos, 0)>=0 && read(fd, &temp, 1)==1) {
            off_t delta = (pos + 1) - base;

            base += delta;
            if (expand) range = (expand <<= 1) - base;
            else range -= delta;
        } else {
            expand = 0;
            range = pos - base;
        }
    } while (range > 0);

    lseek(fd, old, SEEK_SET);

    return base;
}

// Keep reading until full or EOF
ssize_t readall(int fd, void *buf, size_t len)
{
    size_t count = 0;

    while (count<len) {
        int i = read(fd, (char *)buf+count, len-count);
        if (!i) break;
        if (i<0) return i;
        count += i;
    }

    return count;
}

// Die unless we can allocate memory.
void *xmalloc(size_t size)
{
    void *ret = malloc(size);
    if (ret == NULL) {
        perror("xmalloc");
        fprintf(stderr, "failed to allocate %zu bytes", size);
    }
    return ret;
}

// Die unless we can change the size of an existing allocation, possibly
// moving it.  (Notice different arguments from libc function.)
//
// the input pointer is still valid if reallocation failed.
void *xrealloc(void *ptr, size_t size)
{
    void * newptr = realloc(ptr, size);
    if (newptr == NULL) {
        perror("xrealloc");
        fprintf(stderr, "failed to reallocate ptr to %zu bytes", size);
    }
    return newptr;
}

// Die unless we can change the size of an existing allocation, possibly
// moving it.  (Notice different arguments from libc function.)
//
// the input pointer, if not NULL, is freed if reallocation failed.
void *xreallocf(void *ptr, size_t size)
{
    void * newptr = realloc(ptr, size);
    if (newptr == NULL) {
        perror("xrealloc");
        fprintf(stderr, "failed to reallocate ptr to %zu bytes", size);
        if (ptr != NULL) free(ptr);
    }
    return newptr;
}

char *readfd(int fd, char *ibuf, off_t *plen)
{
    off_t len, rlen;
    char *buf, *rbuf;

    // Unsafe to probe for size with a supplied buffer, don't ever do that.
    if ((ibuf ? !*plen : *plen)) {
        puts("bad readfileat");
        return NULL;
    }

    // If we dunno the length, probe it. If we can't probe, start with 1 page.
    if (!*plen) {
        if ((len = fdlength(fd))>0) *plen = len;
        else len = 4096;
    } else len = *plen-1;

    if (!ibuf) {
        buf = xmalloc(len+1);
        if (buf == NULL) return NULL;
    } else buf = ibuf;

    for (rbuf = buf;;) {
        rlen = readall(fd, rbuf, len);
        if (*plen || rlen<len) break;

        // If reading unknown size, expand buffer by 1.5 each time we fill it up.
        rlen += rbuf-buf;
        buf = xreallocf(buf, len = (rlen*3)/2);
        if (buf == NULL) return NULL;
        rbuf = buf+rlen;
        len -= rlen;
    }
    *plen = len = rlen+(rbuf-buf);

    if (rlen<0) {
        if (ibuf != buf) free(buf);
        buf = 0;
    } else buf[len] = 0;

    return buf;
}

// Read contents of file as a single nul-terminated string.
// measure file size if !plen
// Returns amount of data read in *plen, this is allocated and must be freed
char *readfileat(int dirfd, const char *name, off_t *plen)
{
    char * ibuf = NULL;
    if (-1 == (dirfd = openat(dirfd, name, O_RDONLY))) return 0;

    ibuf = readfd(dirfd, ibuf, plen);
    close(dirfd);

    return ibuf;
}

// basename() can modify its argument or return a pointer to a constant string
// This just gives after the last '/' or the whole stirng if no /
const char *getbasename(const char *name)
{
    char *s = strrchr(name, '/');

    if (s) return s+1;

    return name;
}

// Die unless we can allocate enough space to sprintf() into.
char * xsprintf(char *format, ...)
{
    va_list va, va2;
    size_t len;
    char *ret;

    va_start(va, format);
    va_copy(va2, va);

    // How long is it?
    len = (size_t) vsnprintf(0, 0, format, va);
    len++;
    va_end(va);

    // Allocate and do the sprintf()
    ret = xmalloc(len);
    if (ret == NULL) {
        va_end(va2);
        return NULL;
    }
    vsnprintf(ret, len, format, va2);
    va_end(va2);

    return ret;
}

// matches a process name with available pids located inside the /proc directory
//
//
//  params:
//     name              - the name of the process to find
//
//     pid_list          - a NULL terminated list of pid's found,
//                           if allocate_pid_list is true, then this MUST point
//                           to a NULL pointer:
//                                  // ...
//                                  pid_t *nullptr = NULL;
//                                  pidof(name, &nullptr, count, true); // allowed
//                                  // ...
//                                  pid_t *non_nullptr = malloc(1);
//                                  pidof(name, &name, count, true); // error
//                                  pidof(name, &name, count, false); // allowed
//                                  free(non_nullptr); // malloc(1);
//
//     pid_count         - a count of the total number of pids
//                           contained in pid_list
//                           NOTE: pid_list[pid_count] == NULL
//
//     allocate_pid_list - if this is true, then pid_list is allocated and set as normal
//                           otherwise pid_list is not set and is not allocated
//                           this is useful if you only want a pid count
//
//
// returns:
//     true  if a full list has been obtained
//
//     false if an incomplete list has been obtained
bool pidof__(const char *name, pid_t ** pid_list, int * pid_count, bool allocate_pid_list) {
    if (*pid_list != NULL && allocate_pid_list) {
        fprintf(stderr, "pid_list must not point to a non NULL (nullptr in C++) value");
        return false;
    }

    DIR *dp = NULL;
    struct dirent *entry;

    if (!(dp = opendir("/proc"))) {
        perror("no /proc");
        return false;
    }

    *pid_count = 0;

    while ((entry = readdir(dp))) {
        unsigned pid = (unsigned int) atoi(entry->d_name);
        if (!pid) continue;

        struct stat st1, st2;
        const char *bb = getbasename(name);
        off_t len = (off_t) strlen(bb);

        // If we have a path to existing file only match if same inode
        if (bb!=name && !stat(name, &st1)) {
            char * buf = xsprintf("/proc/%u/exe", pid);
            if (buf == NULL) goto fail;
            int ret = stat(buf, &st2);
            free(buf);
            if (ret == -1) continue;
            if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino) continue;
            goto match;
        }

        // Nope, gotta read command line to confirm
        char * cmd = xsprintf("/proc/%u/cmdline", pid);
        if (cmd == NULL) goto fail;
        len = 0;
        char * cmdbuf = readfileat(AT_FDCWD, cmd, &len);
        free(cmd);
        if (cmdbuf == NULL) continue;
        // readfile only guarantees one null terminator and we need two
        // (yes the kernel should do this for us, don't care)
        cmdbuf[len] = 0;
        if (!strcmp(bb, getbasename(cmdbuf))) {
            free(cmdbuf);
            goto match;
        }
        continue;
        match:
            // build a NULL terminated list
            *pid_count = *pid_count + 1;
            if (allocate_pid_list) {
                if (*pid_list == NULL) {
                    *pid_list = xmalloc(sizeof(pid_t) * 2);
                    if (*pid_list == NULL) {
                        *pid_count = 0;
                        goto fail;
                    }
                    (*pid_list)[*pid_count] = NULL;
                } else {
                    // do not destroy the current list on failure
                    pid_t *pid_listNew = xrealloc(*pid_list, sizeof(pid_t) * ((*pid_count) + 1));
                    if (pid_listNew == NULL) goto fail;
                    *pid_list = pid_listNew;
                    (*pid_list)[*pid_count] = NULL;
                }
                (*pid_list)[(*pid_count) - 1] = pid;
            }
            break;
        fail:
            closedir(dp);
            return false;
    }
    closedir(dp);
    return true;
}

bool pidof(const char *name, int *pid_count) {
    pid_t * nullptr = NULL;
    return pidof__(name, &nullptr, pid_count, false);
}

bool pidof_with_pid_list(const char *name, pid_t **pid_list, int *pid_count) {
    return pidof__(name, pid_list, pid_count, true);
}
