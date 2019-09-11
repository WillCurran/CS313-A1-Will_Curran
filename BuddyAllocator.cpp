/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
    // assign basic block size which is the next biggest pwr of 2
    setBasicBlockSize(_basic_block_size);
    // assign memory length
    setTotalMemoryLength(_total_memory_length);
	// allocate a chunk of memory
    memory_block_head = new char[total_memory_length];
    largest_block_index = (int) log2(total_memory_length/basic_block_size) - 1;
    // populate the FreeList with linked lists
    for(int i = 0; i <= largest_block_index; i++) {
        free_list[i] = LinkedList();
        free_list[i].insert(NULL);
    }
    // populate the last linked list with a block
    BlockHeader* b = new BlockHeader();
    b->free = true;
    b->size = total_memory_length;
    b->next = NULL;
    free_list[largest_block_index].insert(b);
}

BuddyAllocator::~BuddyAllocator (){
    delete[] memory_block_head;
}

void BuddyAllocator::setBasicBlockSize(int input_size) { // assumes a power of 2 is entered
    basic_block_size = (unsigned int) input_size;
}

void BuddyAllocator::setTotalMemoryLength(int input_size) { // assumes a power of 2 is entered
    total_memory_length = (unsigned int) input_size;
}

char* BuddyAllocator::alloc(int _length) {
    // go to smallest slot possible in free list and search until a free block is found
    int smallest_index = getFreeListIndex(_length);
    if(!free_list[smallest_index].empty()) {
        BlockHeader* block = free_list[smallest_index].back();
        free_list[smallest_index].remove(block);
        // return raw block
        return getRawFromHeader(block);
    }
    for(int i = smallest_index + 1; i <= largest_block_index; i++) {
        if(!free_list[i].empty()) {
            BlockHeader* block = free_list[smallest_index].back();
            free_list[smallest_index].remove(block);
            // recursively split block
            // return raw block
            return getRawFromHeader(split(block));
        }
    }
    cout << "attempting to alloc more memory than is available in total." << endl;
    return NULL;
}

int BuddyAllocator::free(char* _a) {
  // get the blockheader, add it to the free list or merge with its buddy in the free list
  return 0;
}

void BuddyAllocator::debug (){
  
}

