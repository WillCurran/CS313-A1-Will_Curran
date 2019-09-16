/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include "DebugHelper.h"
#include <iostream>
#include <math.h>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
    // assign basic block size which is the next biggest pwr of 2
    setBasicBlockSize(_basic_block_size); // DONE
    // assign memory length
    setTotalMemoryLength(_total_memory_length); // DONE
    // allocate a chunk of memory
    memory_block_head = new char[total_memory_length]; // DONE
    largest_block_index = getFreeListIndex(total_memory_length); // ?
    // populate the FreeList with linked lists
    for(int i = 0; i <= largest_block_index; i++) {
        free_list[i] = LinkedList();
        free_list[i].insert(NULL);
    }
    // populate the last linked list with a block
    BlockHeader* b = (BlockHeader*) memory_block_head;
    b->size = total_memory_length;
    b->next = NULL;
    free_list[largest_block_index].insert(b);
}

BuddyAllocator::~BuddyAllocator (){
    delete[] memory_block_head;
}

BlockHeader* BuddyAllocator::getbuddy (BlockHeader * addr) {
    pointer_arithmetic_t relative_addr = (pointer_arithmetic_t) addr;
    pointer_arithmetic_t memory_head = (pointer_arithmetic_t) memory_block_head;
    pointer_arithmetic_t size_of_block = (pointer_arithmetic_t) addr->size;
    relative_addr -= memory_head; // subtract off the start address of the block
    // get 0-based xor of the block's address and its size, then add the head address back & cast to BH*
    return (BlockHeader*) ((relative_addr ^ size_of_block) + memory_head);
}

bool BuddyAllocator::arebuddies (BlockHeader* block1, BlockHeader* block2) {
    return getbuddy(block1) == block2;
}

BlockHeader* BuddyAllocator::merge (BlockHeader* block1, BlockHeader* block2) {
    assert(block1 && block2);
    assert(block1->size == block2->size);
    int size_of_these_blocks = block1->size;
    int free_list_index = getFreeListIndex(size_of_these_blocks);
    // if either blocks were in free list, remove them
    if(!free_list[free_list_index].empty()) {
        free_list[free_list_index].remove(block1);
        free_list[free_list_index].remove(block2);
    }
    pointer_arithmetic_t b1 = (pointer_arithmetic_t) block1;
    pointer_arithmetic_t b2 = (pointer_arithmetic_t) block2;
    if(b1 > b2) { // whichever block is first, return it with double the size
        block1->size *= 2;
        free_list[free_list_index + 1].insert(block1); // add big block in the next slot of free list
        return block1;
    }
    block2->size *= 2;
    free_list[free_list_index + 1].insert(block2); // add big block in the next slot of free list
    return block2;
}

BlockHeader* BuddyAllocator::split (BlockHeader* block) {
    pointer_arithmetic_t block_ptr = (pointer_arithmetic_t) block; // will advance this to the buddy position
    block_ptr += (pointer_arithmetic_t) (block->size - BLOCKHEADER_SIZE); // now at front of new buddy
    BlockHeader* new_buddy = (BlockHeader*) block_ptr;
    block->size /= 2;
    new_buddy->size = block->size;
    new_buddy->next = NULL;
    // do not add the new split block to the free list because it is about to be used
    return new_buddy;
}

char* BuddyAllocator::getRawFromHeader(BlockHeader* block) {
    pointer_arithmetic_t raw = (pointer_arithmetic_t) block;
    raw += BLOCKHEADER_SIZE;
    return (char*) raw;
}
// advance the BlockHeader pointer to its raw memory block

BlockHeader* BuddyAllocator::getHeaderFromRaw(char* raw_block) {
    pointer_arithmetic_t header = (pointer_arithmetic_t) raw_block;
    header -= BLOCKHEADER_SIZE;
    return (BlockHeader*) header;
}
// backtrack from raw memory block pointer to its BlockHeader

int BuddyAllocator::getFreeListIndex(int requested_size) {
    return (int) log2(requested_size/basic_block_size);
}
// get an index corresponding to a slot in the free_list which could offer enough memory for the user

void BuddyAllocator::setBasicBlockSize(int input_size) { // assumes a power of 2 is entered
    basic_block_size = (uint) input_size;
}

void BuddyAllocator::setTotalMemoryLength(int input_size) { // assumes a power of 2 is entered
    total_memory_length = (uint) input_size;
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
    if(!_a) {
        return -1;
    }
    BlockHeader* block = getHeaderFromRaw(_a);
    BlockHeader* buddy = getbuddy(block);
    int free_list_index = getFreeListIndex(block->size);
    if(free_list[free_list_index].includes(buddy)) {
        merge(block, buddy);
    } else {
        free_list[free_list_index].insert(block);
    }
    return 0;
}

void BuddyAllocator::debug () {
    cout << "Test Battery:" << endl;
    runAllTests();
}
