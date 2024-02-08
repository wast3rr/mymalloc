#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

#define MEMLENGTH 512
static double memory[MEMLENGTH];

#define heap ((char *) memory)

// mymalloc returns a void function pointer to the client's created payload
void *mymalloc(size_t size, char *file, int line) {
   int width = (size+7) & 8;
   
  if (size > MEMLENGTH) {
    printf("Not enough space in heap for malloc call in %s on line %d.", file, line);
    return NULL;
  } 

  if (* (int *) heap == 0) {
      * (int *) heap = 4088;
      * (int *) (heap+4) = 0;
  }

  char *finder = heap;
  int currsize = * (int *) finder;
  int alloc = * (int *) (finder+4);

  while ((currsize < size || alloc == 1) && finder < heap+4096) {
        finder = finder+8+currsize;
        currsize = * (int *) finder;
        alloc = * (int *) (finder+4);
  }

  if (finder >= (heap+4096)) {
      printf("Not enough contiguous space available for malloc call in %s on line %d.", file, line);
      return NULL;
  }
    
  * (int *) finder = width;
  * (int *) (finder+4) = 1;
  void *ptr = (void *) (finder+8);

  finder = finder+8+width;
  if (* (int *) (finder+4) == 0) {
      * (int *) finder = currsize-8-width;
   }

  return ptr;
};



void myfree(void *ptr, char *file, int line);
