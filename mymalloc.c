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
    int *header = (int *) ptr;
    header[0] = size;
    header[1] = alloc;
}



// mymalloc() returns a void function pointer to the client's created payload or NULL if it fails
void *mymalloc(size_t size, char *file, int line) {
    // this gets the closest multiple of 8 from size
    int alignedsize = (size+7) & -8;
    
    if (size > MEMLENGTH * 8 - HEADERLENGTH) {
        printf("malloc error: not enough space in heap for desired byte amount (%s:%d)\n", file, line);
        return NULL;
    } 
    
    // if the heap is uninitialized, create the first header
    if (* (int *) heap == 0) {
        setheader(heap, 4088, 0);
    }
   
    char *currptr = heap;                   // will be used to traverse the heap
    int *currheader = (int *) currptr;      // the first index of the header int pointer is the size, the second is the allocation satus
  
    int currsize = currheader[0];           // size of current header
    int alloc = currheader[1];              // allocation status of current header (0 for unallocated, 1 for allocated)
  
    // While current header size is too small or payload is allocated and currptr is still within heap range, proceed to nextptr chunk header
    while ((currsize < size || alloc == 1) && currptr < heap+4096) {
          currptr = currptr + HEADERLENGTH + currsize;
          currheader = (int *) currptr;
          currsize = currheader[0];
          alloc = currheader[1];
    }
 
    // if ptr exceeds heap range, we don't have enough contiguous space
    if (currptr >= (heap+4096)) {
        printf("malloc error: not enough contiguous space in heap for byte amount (%s:%d)\n", file, line);
        return NULL;
    }

    // Set current header of new object to payload's size and the alloc status to allocated, then creates void pointer to payload
    setheader(currptr, alignedsize, 1);
    void *ptr = (void *) (currptr + HEADERLENGTH);


    // Creates the nextptr header for free chunk to the right of allocated payload, as long as there is free space and not another allocated chunk nextptr to the space
    currptr = currptr + HEADERLENGTH + alignedsize;
    currheader = (int *) currptr;
    if (currheader[1] == 0) {
        currheader[0] = currsize - HEADERLENGTH - alignedsize;
    }

    return ptr;
};



// myfree() sets the chosen chunk to unallocated if it exists/was created by malloc, and coalesces the chunk with adjacent free chunks
void myfree(void *ptr, char *file, int line){
    char *currptr = heap;                   // points to the current header being checked/focused on
    int *currheader = (int *) currptr;      
    char *prevptr = currptr;                // points to the header before the header currptr is pointing at 
    
    int currsize = currheader[0];           // size of current chunk
    
    if ((char *) ptr >= heap+4096 || (char *) ptr < heap) {
        printf("free error: address not obtained from malloc() (%s:%d)\n", file, line);
        return;
    }

    // will iterate until currptr gets to a pointer that matches the provided void pointer,
    // or when it is outside of the heap's memory range
    while ((void *) (currptr + HEADERLENGTH) != ptr && currptr < heap+4096) {
        currsize = currheader[0];
        prevptr = currptr;
        currptr = currptr+8+currsize;
        currheader = (int *) currptr;
    }

    currsize = currheader[0];
    
    // if the pointer isn't found by the prevptrious loop before pointer gets out of range,
    // we can assume that the pointer is not at the beginning of a chunk
    if (currptr >= heap+4096) {
        printf("free error: pointer provided not at beginning of a block (%s:%d)\n", file, line);
        return;
    }
    
    // if the pointer has already been freed, then free fails
    if (currheader[1] == 0) {
        printf("free error: double free call on pointer (%s:%d)\n", file, line);
        return;
    }
    
    currheader[1] = 0;                                  // sets currptr's status to unallocated

    char *nextptr = currptr + HEADERLENGTH + currsize;  // points to the nextptr header from currptr
    int *nextheader = (int *) nextptr;
    int *prevheader = (int *) prevptr;
    
    int prevptrFree = 0;                                // tracks if header previous to currptr is freed (0) or alocated (1)
    
    // if prevptr header is also unallocated, the previous and current payloads are coalesced
    if (prevheader[1] == 0 && prevptr != currptr) {
        prevptrFree = 1;
        int newsize = currsize + prevheader[0] + HEADERLENGTH; // need the additional 8 here to account for old current header
        prevheader[0] = newsize;
        currheader[0] = 0;
    }
    
    // if the nextptr header is also unallocated, coalesce
    if (nextheader[1] == 0) {
        // If prevptr was also unallocateed, then all three are coalesced here
        if (prevptrFree == 1) {
            prevheader[0] = prevheader[0] + nextheader[0] + HEADERLENGTH;
            nextheader[0] = 0;
        // If prevptr was not unallocated, then the coalescing begins with currptr (current header) instead
        } else {
            currheader[0] = currsize + nextheader[0] + HEADERLENGTH;
            nextheader[0] = 0;
        }
    }
};



// detectleaks() can be called by the client at program end to detect any chunks still allocated
void detectleaks() {
    char *currptr = heap;
    int *currheader = (int *) currptr;
    int currsize = currheader[0];

    if (currheader[0] != 4088) {
        printf("WARNING: The following memory chunks are still allocated:\n");
    } else {
        return;
    }

    while (currptr < heap+4096) {
        currsize = currheader[0];

        if (currheader[1] == 1) {
            printf("Address: %p, Size: %d\n", currptr, currsize);
        }

        currptr = currptr + currsize + HEADERLENGTH;
        currheader = (int *) currptr;
    }
};
