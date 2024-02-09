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
      printf("malloc error: not enough space in heap for desired byte amount (%s:%d)\n", file, line);
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
      printf("malloc error: not enough contiguous space in heap for byte amount (%s:%d)\n", file, line);
      return NULL;
  }

  // Set current header of new object to payload's size and the alloc status to allocated, then creates void pointer to payload
  * (int *) finder = width;
  * (int *) (finder+4) = 1;
  void *ptr = (void *) (finder+8);
  //printf("Successfully allocated chunk with width %d based on requested %ld at %p.\n", width, size, finder+8);


  // Creates the next header for free chunk to the right of allocated payload, as long as there is free space and not another allocated chunk next to the space
  finder = finder+8+width;
  if (* (int *) (finder+4) == 0) {
      * (int *) finder = currsize-8-width;
   }

  return ptr;
};


void myfree(void *ptr, char *file, int line){
    char *finder = heap;                // This pointer points to the current header being checked/focused on
    char *prev = finder;                  // This will point to the header before the header finder is pointing at 
    int currsize = * (int *) finder;    // The size of the payload finder is currently at
    
    if ((char *) ptr >= heap+4096) {
        printf("free error: address not obtained from malloc() (%s:%d)\n", file, line);
        exit(EXIT_FAILURE);
    }

    // This loop will iterate until finder gets to a pointer that matches the provided void pointer,
    // or when it is outside of the heap's memory range
    while ((void *) (finder+8) != ptr && finder < heap+4096) {
        currsize = * (int *) finder;
        prev = finder;
        finder = finder+8+currsize;
    }

    currsize = * (int *) finder;
    
    // If the pointer isn't found by the previous loop before pointer gets out of range,
    // we can assume that the pointer was not created by malloc 
    if (finder >= heap+4096) {
        printf("free error: pointer provided not at beginning of a block (%s:%d)\n", file, line);
        return;
    }
    
    // If the pointer has already been freed, then free fails
    if (* (int *) (finder+4) == 0) {
        printf("free error: double free call on pointer (%s:%d)\n", file, line);
        return;
    }

    char *next = finder+8+currsize; // points to the next header from finder
    int prevFree = 0;               // tracks if header previous to finder is freed (1) or alocated (1)
    * (int *) (finder+4) = 0;       // Sets finder's header to unallocated
    //printf("Successfully freed chunk at %p, based on user request %p.\n", finder, ptr);

    // If previous header is also unallocated, the previous and current payloads are coalesced
    if ( * (int *) (prev+4) == 0 && prev != finder) {
        prevFree = 1;
        int newsize = currsize + (* (int *) prev) + 8; // need the additional 8 here to account for old current header
        * (int *) prev = newsize;
        * (int *) (finder) = 0;
        //printf("Previous chunk coalesced at %p.\n", prev);
    }
    
    // If the next header is also unallocated, coalesce
    if ( * (int *) (next+4) == 0) {
        // If prev was also unallocateed, then all three are coalesced here
        if (prevFree == 1) {
            * (int *) prev = (* (int *) prev) + (* (int *) next) + 8;
            * (int *) next = 0;
        // If prev was not unallocated, then the coalescing begins with finder (current header) instead
        } else {
            * (int *) finder = currsize + (* (int *) next) +8;
            * (int *) next = 0;
        }
        //printf("Next chunk coalesced at %p.\n", next);
    }
};
