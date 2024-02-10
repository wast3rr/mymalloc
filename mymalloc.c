#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

#define MEMLENGTH 512
static double memory[MEMLENGTH];

#define heap ((char *) memory)
#define HEADERLENGTH 8



// setheader() sets the header at ptr to the chosen size and allocation status
static void setheader(char *ptr, int size, int alloc) {
    * (int *) ptr = size;
    * (int *) (ptr + 4) = alloc;
}



// mymalloc() returns a void function pointer to the client's created payload or NULL if it fails
void *mymalloc(size_t size, char *file, int line) {
    // this gets the closest multiple of 8 from size
   int width = (size+7) & -8;
   
  if (size > MEMLENGTH * 8) {
      printf("malloc error: not enough space in heap for desired byte amount (%s:%d)\n", file, line);
      return NULL;
  } 
    
  // if the heap is uninitialized, create the first header
  if (* (int *) heap == 0) {
      setheader(heap, 4088, 0);
  }
   
  char *finder = heap;                  // will be used to traverse the heap
  int currsize = * (int *) finder;      // size of current header
  int alloc = * (int *) (finder+4);     // allocation status of current header (0 for unallocated, 1 for allocated)
  
  // While current header size is too small or payload is allocated and finder is still within heap range, proceed to next chunk header
  while ((currsize < size || alloc == 1) && finder < heap+4096) {
        finder = finder + HEADERLENGTH + currsize;
        currsize = * (int *) finder;
        alloc = * (int *) (finder+4);
  }
    
  if (finder >= (heap+4096)) {
      printf("malloc error: not enough contiguous space in heap for byte amount (%s:%d)\n", file, line);
      return NULL;
  }

  // Set current header of new object to payload's size and the alloc status to allocated, then creates void pointer to payload
  setheader(finder, width, 1);
  void *ptr = (void *) (finder + HEADERLENGTH);


  // Creates the next header for free chunk to the right of allocated payload, as long as there is free space and not another allocated chunk next to the space
  finder = finder + HEADERLENGTH + width;
  if (* (int *) (finder+4) == 0) {
      * (int *) finder = currsize - HEADERLENGTH - width;
   }

  return ptr;
};



// myfree() sets the chosen chunk to unallocated if it exists/was created by malloc, and coalesces the chunk with adjacent free chunks
void myfree(void *ptr, char *file, int line){
    char *finder = heap;                // points to the current header being checked/focused on
    char *prev = finder;                // points to the header before the header finder is pointing at 
    int currsize = * (int *) finder;    // size of current chunk
    
    if ((char *) ptr >= heap+4096 || (char *) ptr < heap) {
        printf("free error: address not obtained from malloc() (%s:%d)\n", file, line);
        return;
    }

    // will iterate until finder gets to a pointer that matches the provided void pointer,
    // or when it is outside of the heap's memory range
    while ((void *) (finder + HEADERLENGTH) != ptr && finder < heap+4096) {
        currsize = * (int *) finder;
        prev = finder;
        finder = finder+8+currsize;
    }

    currsize = * (int *) finder;
    
    // if the pointer isn't found by the previous loop before pointer gets out of range,
    // we can assume that the pointer is not at the beginning of a chunk
    if (finder >= heap+4096) {
        printf("free error: pointer provided not at beginning of a block (%s:%d)\n", file, line);
        return;
    }
    
    // if the pointer has already been freed, then free fails
    if (* (int *) (finder+4) == 0) {
        printf("free error: double free call on pointer (%s:%d)\n", file, line);
        return;
    }

    char *next = finder + HEADERLENGTH + currsize;  // points to the next header from finder
    int prevFree = 0;                               // tracks if header previous to finder is freed (0) or alocated (1)
    * (int *) (finder+4) = 0;                       // sets finder's status to unallocated

    // if previous header is also unallocated, the previous and current payloads are coalesced
    if ( * (int *) (prev+4) == 0 && prev != finder) {
        prevFree = 1;
        int newsize = currsize + (* (int *) prev) + HEADERLENGTH; // need the additional 8 here to account for old current header
        * (int *) prev = newsize;
        * (int *) (finder) = 0;
    }
    
    // if the next header is also unallocated, coalesce
    if ( * (int *) (next+4) == 0) {
        // If prev was also unallocateed, then all three are coalesced here
        if (prevFree == 1) {
            * (int *) prev = (* (int *) prev) + (* (int *) next) + HEADERLENGTH;
            * (int *) next = 0;
        // If prev was not unallocated, then the coalescing begins with finder (current header) instead
        } else {
            * (int *) finder = currsize + (* (int *) next) + HEADERLENGTH;
            * (int *) next = 0;
        }
    }
};
