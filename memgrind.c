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
    printf("Avg time to create and deallocate 120 one-byte chunks: %f ms\n", totaltime/50.0);
    
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
    printf("Avg time to randomly allocate/deallocate a one-byte chunk for 120 allocations: %f ms\n", totaltime/50.0); 

}
