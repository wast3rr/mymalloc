#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

#define MEMLENGTH 512
static double memory[MEMLENGTH];

// Heap will be the byte pointer used to reference memory heap
#define heap ((char *) memory)

// mymalloc returns a void function pointer to the client's created payload
void *mymalloc(size_t size, char *file, int line) {
    // This gets the closest multiple of 8 from size
   int width = (size+7) & -8;
   
   // If the user asks for more space than is in the memory heap, return error
  if (size > MEMLENGTH*8) {
      printf("Not enough space in heap for malloc call in %s on line %d.\n", file, line);
      return NULL;
  } 
    
  // If the heap is uninitialized, create the first header
  if (* (int *) heap == 0) {
      * (int *) heap = 4088;
      * (int *) (heap+4) = 0;
  }
   
  char *finder = heap;                  // Finder point that will be used to traverse the heap
  int currsize = * (int *) finder;      // Current size of payload based on header finder is currently on
  int alloc = * (int *) (finder+4);     // Allocation status of header finder is currently on (0 for unallocated, 1 for allocated)
  
  // While current header size is too small or payload is allocated and finder is still within heap range, proceed to next chunk header
  while ((currsize < size || alloc == 1) && finder < heap+4096) {
        finder = finder+8+currsize;
        currsize = * (int *) finder;
        alloc = * (int *) (finder+4);
  }

  if (finder >= (heap+4096)) {
      printf("Not enough contiguous space available for malloc call in %s on line %d.\n", file, line);
      return NULL;
  }

  // Set current header of new object to payload's size and the alloc status to allocated, then creates void pointer to payload
  * (int *) finder = width;
  * (int *) (finder+4) = 1;
  void *ptr = (void *) (finder+8);
  
  // Creates the next header for free chunk to the right of allocated payload, as long as there is free space and not another allocated chunk next to the space
  finder = finder+8+width;
  if (* (int *) (finder+4) == 0) {
      * (int *) finder = currsize-8-width;
   }

  return ptr;
};


void myfree(void *ptr, char *file, int line){
    char *finder = heap;
    char *prev = finder;
    int currsize = * (int *) finder;

    while ((void *) (finder+8) != ptr && finder < heap+4096) {
        currsize = * (int *) finder;
        prev = finder;
        finder = finder+8+currsize;
    }

    if (finder >= heap+4096) {
        printf("Error: Pointer used in free() in %s on line %d not created by malloc.\n", file, line);
        exit(EXIT_FAILURE);
    }

    if (* (int *) (finder+4) == 0) {
        printf("Error: Pointer used in free() in %s on line %d is not currently allocated.\n", file, line);
        exit(EXIT_FAILURE);
    }
    
    char *next = finder+8+currsize;
    int prevFree = 0;
    * (int *) (finder+4) = 0;
    

    if ( * (int *) (prev+4) == 0) {
        prevFree = 1;
        int newsize = currsize + (* (int *) prev) + 8; // need the additional 8 here to account for old current header
        * (int *) prev = newsize;
        * (int *) (finder) = 0;
    }
    
    if ( * (int *) (next+4) == 0) {
        if (prevFree == 1) {
            * (int *) prev = (* (int *) prev) + (* (int *) next) + 8;
            * (int *) next = 0;
        } else {
            * (int *) finder = currsize + (* (int *) next) +8;
            * (int *) next = 0;
        }
    }
};
