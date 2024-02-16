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
    int *q;
    int *k;

    switch (test) {
        // ERROR TESTS
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
            q = malloc(1);
            break;
        // not enough total heap space malloc error
        case 5:
            p = malloc(4096);
            break;

        // GENERAL CORRECTNESS TESTS (these should not throw errors unless stated otherwise)
        // tests that malloc(0) causes no issues for a future malloc call
        case 6:
            p = malloc(0);
            q = malloc(4088);
            break;
        // tests to make sure client data is never corrupted even after malloc and free errors
        // should print two errors and "1, 2" if successful
        case 7: 
            p = malloc(8);
            p[0] = 1;
            p[1] = 2;

            for (int i = 0; i < 50; i++) {
                q = malloc(4000);
                free(q);
            }
            free(q); //double free error
            q = malloc(5000); // not enough space error

            printf("%d, %d\n", p[0], p[1]);
            break;
        // tests the detectleaks() method
        case 8:
            p = malloc(1);
            q = malloc(1);
            detectleaks();
            break;
        // checks that if you free a block, you can allocate something there even if there are allocated blocks to the left and right
        case 9:
            p = malloc(1);
            q = malloc(16);
            k = malloc(4040);
            free(q);
            q = malloc(16);
            break;
    } 

    return EXIT_SUCCESS;
}
