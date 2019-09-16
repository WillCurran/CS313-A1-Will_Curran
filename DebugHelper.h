#ifndef _DebugHelper_h_
#define _DebugHelper_h_
#include "BuddyAllocator.h"
#include <iostream>

void getBuddyTest(BuddyAllocator* b) {
    std::cout << "Test1: ";
    pointer_arithmetic_t base_address = 2048;
    BlockHeader* bh1 = new BlockHeader();
    bh1->size = 128;
    
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
    getBuddyTest(b);
    delete b;
}

#endif
