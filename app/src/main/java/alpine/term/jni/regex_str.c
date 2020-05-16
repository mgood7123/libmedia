//
// Created by konek on 7/9/2019.
//

#include <stdlib.h>
#include <asm/fcntl.h>
#include <fcntl.h>
#include "regex_str.h"

char * string_register_pointer = NULL;
int STR_INDENT_LEVEL = 0;
int STR_INDENT_SIZE = 4;

void str_reg(char *str) {
    string_register_pointer = str;
}

int getbitgroupcount(int bit) {
    char * n = (char *) malloc(256);
    int bit_ = snprintf(n, 0, "%d", bit);
    memset(n, 0, 256);
    free(n);
    return bit_;
}

struct QNode* newNode(int16_t type)
{
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->type = type;
    temp->next = NULL;
    return temp;
}

struct Queue *createQueue()
{
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

void store_asm(struct Queue **qq, int16_t type)
{
    if (*qq == NULL)
        *qq = createQueue();

    // Create a new LL node
    struct QNode *temp = newNode(type);

    // If queue is empty, then new node is front and rear both
    if ((*qq)->rear == NULL)
    {
        (*qq)->front = (*qq)->rear = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    temp->next = (*qq)->rear;
    (*qq)->rear = temp;
}

struct QNode * load_asm(struct Queue **qq)
{
    if ((*qq) == NULL)
        return NULL;

    // If queue is empty, return NULL.
    if ((*qq)->rear == NULL)
        return NULL;

    // Store previous front and move front one node ahead
    struct QNode *temp = (*qq)->rear;
    (*qq)->rear = (*qq)->rear->next;

    // If front becomes NULL, then change rear also as NULL
    if ((*qq)->rear == NULL)
        (*qq)->front = NULL;
    return temp;
}

int reverseBool(int val) {
    if (val == 1 || val == 0) return val^1;
    else return val;
}

size_t read__(const char *file, char **p) {
    int fd;
    size_t len = 0;
    char *o = NULL;
    *p = NULL;
    if (!(fd = open(file, O_RDONLY)))
    {
        puts("open() failure");
        return 0;
    }
    len = (size_t) lseek(fd, 0, SEEK_END);
    lseek(fd, 0, 0);
    if (!(o = (char *) malloc(len))) {
        puts("failure to malloc()");
    }
    if ((read(fd, o, len)) == -1) {
        puts("failure to read()");
    }
    int cl = close(fd);
    if (cl < 0) {
        printf("cannot close \"%s\", returned %d\n", file, cl);
        return 0;
    }
    *p = o;
    return len;
}

char * filetostring(const char * file) {
    char * buf;
    size_t size;
    size = read__(file, &buf);
    buf[size] = '\0';
    return strdup(buf);
}
