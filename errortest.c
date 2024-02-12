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
    // contiguous space error
    int *test = malloc(4080);
    int *heaperror = malloc(1);

    // free not at the beginning of a block error
    free(test + 1);
    free(test);

    // not enough heap space error
    heaperror = malloc(4097);

    // double free error
    heaperror = malloc(10);
    free(heaperror);
    free(heaperror);

    // address not obtained by malloc error
    int x = 0;
    free(&x);

    return EXIT_SUCCESS;
}
