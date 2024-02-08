#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

#define MEMSIZE 4096
#define HEADERSIZE 8

int main(int argc, char **argv) {
    void *test = malloc(750);
    void *test2 = malloc(8);
    void *test3 = malloc(1000);
    // void *test4 = malloc(3500); // Should print error if not commented out uh oh this is not creating an error nvm i fixed it
    return 0;
}
