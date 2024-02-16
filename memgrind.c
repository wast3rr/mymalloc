#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#ifndef REALMALLOC
#include "mymalloc.h"
#endif

int main(int argc, char **argv) {
    struct timeval start, end;
    double totaltime = 0;
    
    // first provided stress test: allocating and deallocating one byte 120 times 
    for (int i = 0; i < 50; i++) {
        gettimeofday(&start, 0);

        for (int j = 0; j < 120; j++) {
            char *test = malloc(1);
            free(test);
        }

        gettimeofday(&end, 0);
        totaltime += ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec-start.tv_usec) / 1000.0);
    }    
    printf("Avg time to allocate and immediately deallocate one byte chunk 120 times:\n%f ms\n", totaltime/50.0);
    
    // second provided stress test: putting 120 one-byte objects into an array and deallocating them
    totaltime = 0;
    for (int i = 0; i < 50; i++) {
        gettimeofday(&start, 0);
         char **onebytes = malloc(120*sizeof(char *));

        for (int i = 0; i < 120; i++) {
            onebytes[i] = (char *) malloc(1);
        }

        for (int i = 0; i < 120; i++) {
            free(onebytes[i]);
        }
        free(onebytes);

        gettimeofday(&end, 0);
        totaltime += ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec-start.tv_usec) / 1000.0);
    }
    printf("Avg time to create and deallocate 120 one-byte chunks:\n%f ms\n", totaltime/50.0);
    
    // third provided stress test: randomly allocated one-byte objects 120 times and freeing them
    totaltime = 0;
    for (int i = 0; i < 50; i++) {
        gettimeofday(&start, 0);
           
        char **onebytes = malloc(120*sizeof(char *));
        int currentalloc = 0;
        int totalalloc = 0;

        while (totalalloc < 120) {
            int r = rand() % 2;
            if (r == 1) {
                onebytes[currentalloc] = malloc(1);
                totalalloc++;
                currentalloc++;
            } else if (r == 0 && currentalloc > 0) {
                currentalloc--;
                free(onebytes[currentalloc]);
            }
        }

        for (int j = 0; j < currentalloc; j++) {
            free(onebytes[j]);
        }

        free(onebytes);

        gettimeofday(&end, 0);
        totaltime += ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec-start.tv_usec) / 1000.0);
    }
    printf("Avg time to randomly allocate/deallocate a one-byte chunk for 120 allocations:\n%f ms\n", totaltime/50.0);

    // fourth stress test: allocate 1000 bytes to three pointers and deallocate them 120 times
    totaltime = 0;
    for (int i = 0; i < 50; i++) {
        gettimeofday(&start, 0);
        char *p, *q, *k;

        for (int j = 0; j < 120; j++) {
            p = malloc(1000);
            q = malloc(1000);
            k = malloc(1000);
            free(p);
            free(q);
            free(k);
        }

        gettimeofday(&end, 0);
        totaltime += ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec-start.tv_usec) / 1000.0);
    }
    printf("Avg time to allocate then immediately deallocate three different 1000 byte pointers 120 times:\n%f ms\n", totaltime/50.0);
    
    // fifth stress test: allocate 1000 bytes to one of three pointers randomly 3 times, then free them, 120 times
    totaltime = 0;
    for (int i = 0; i < 50; i++) {
        gettimeofday(&start, 0);
        char *p, *q, *k;

        for (int j = 0; j < 120; j++) {
            int r = rand() % 3;
            switch (r){
                case 0:
                    p = malloc(1000);
                    q = malloc(1000);
                    k = malloc(1000);
                    break;
                case 1:
                    q = malloc(1000);
                    k = malloc(1000);
                    p = malloc(1000);
                    break;
                default:
                    k = malloc(1000);
                    p = malloc(1000);
                    q = malloc(1000);
                    break;
            }
            free(p);
            free(q);
            free(k);
        }

        gettimeofday(&end, 0);
        totaltime += ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec-start.tv_usec) / 1000.0);
    }
    printf("Avg time to randomly allocate three pointers then deallocate them 120 times:\n%f ms\n", totaltime/50.0);
   

    return EXIT_SUCCESS;
}

