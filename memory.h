#pragma once
#if __STDC_HOSTED__
#include <stdint.h>
#include <stddef.h>
typedef uint32_t u32;
extern char heap[524288];
#else
#include "utils.h"
#define NULL ((void*)0)
extern char* heap;
#endif
#define HEAP_ORDER 19


typedef struct Header_{
    u32 used: 1,
        order: 5,
        prev: 13,
        next: 13;
} Header;


extern Header* freeList[HEAP_ORDER+1];

void memory_init();                                // Initialize the memory allocator
void initHeader(Header* h, unsigned order);       // Initialize a memory block header
void* kmalloc(u32 size);                           // Allocate memory of specified size
void kfree(void* ptr);                             // Free allocated memory
void addToFreeList(Header* h);                     // Add a header to the free list
Header* removeFromFreeList(unsigned i);            // Remove a header from the free list by order
void removeThisNodeFromFreeList(Header* h);       // Remove a specific header from the free list
void splitBlockOfOrder(unsigned i);                 // Split a block of memory of a given order
Header* getBuddy(Header* h);                        // Get the buddy of a given memory block
void mergeBlocks(Header* h1, Header* h2);          // Merge two adjacent free blocks
void printFreeList();                               // Debug function to print the free list
u32 getTotalFreeMemory();                           // Get total free memory available
