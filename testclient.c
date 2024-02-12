#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Compile with -DREALMALLOC to use the real malloc() instead of mymalloc()
#ifndef REALMALLOC
#include "mymalloc.h"
#endif

// errortest.c ensures each of the error checks in mymalloc() and myfree() properly trigger
int main(int argc, char **argv){
    if (argc != 2) {
        printf("No test case or multiple test case numbers provided.\n");
        return EXIT_FAILURE;
    }
    int test = atoi(argv[1]);
    int *p;

    switch (test) {
        // tests pointer not obtained from malloc free error
        case 1:
            int x = 0;
            free(&x);
            break;
        // double free error
        case 2:
            p = malloc(25);
            free(p);
            free(p);
            break;
        // pointer not at beginning of block free error
        case 3:
            p = malloc(25);
            free(p+5);
            break;
        // not enough contiguous space malloc error
        case 4:
            p = malloc(4088);
            int *q = malloc(1);
            break;
        // not enough total heap space malloc error
        case 5:
            p = malloc(5000);
            break;
    } 

    return EXIT_SUCCESS;
}
