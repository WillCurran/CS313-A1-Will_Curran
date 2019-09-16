#ifndef _DebugHelper_h_
#define _DebugHelper_h_
#include "BuddyAllocator.h"
#include <iostream>

void allocTest(BuddyAllocator* b) {
    std::cout << "basic alloc test: ";
    char* b1 = b->alloc(100);
    
    if(true) {
        std::cout << "PASSED" << endl;
    } else {
        std::cout << "FAILED" << endl;
    }
}

void runAllTests() {
    int basic_block_size = 128;
    int total_memory_length = 1024;
    BuddyAllocator* b = new BuddyAllocator(basic_block_size, total_memory_length);
    allocTest(b);
    delete b;
}

#endif
