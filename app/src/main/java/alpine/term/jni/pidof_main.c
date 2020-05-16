#include "pidof.h"
#include <stdio.h>

int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("usage: %s <name>\n", argv[0]);
        return -1;
    }
    int pidCount;
    bool returnCode = pidof(argv[1], &pidCount);
    printf("return of pidof: %s\n", returnCode ? "success" : "failure");
    printf("number of pids found by pidof: %d\n", pidCount);
    return 0;
}
