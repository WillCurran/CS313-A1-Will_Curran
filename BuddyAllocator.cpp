/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
using namespace std;

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length){
    assert(_total_memory_length >= _basic_block_size);
    // assign basic block size which is the next biggest pwr of 2
    setBasicBlockSize(_basic_block_size);
    // assign memory length
    setTotalMemoryLength(_total_memory_length);
    // allocate a chunk of memory
    memory_block_head = new char[total_memory_length];
    // getFreeListIndex() returns ceil which is not what we want here
    largest_block_index = getFreeListIndex(total_memory_length);
    // populate the FreeList with linked lists
    for(int i = 0; i <= largest_block_index; i++)
        free_list[i] = LinkedList();
    // populate the last linked list with a block
    BlockHeader* b = (BlockHeader*) memory_block_head;
    b->size = total_memory_length;
    b->next = NULL;
    free_list[largest_block_index].insert(b);
}

BuddyAllocator::~BuddyAllocator (){
    delete[] memory_block_head;
}

BlockHeader* BuddyAllocator::getbuddy (BlockHeader* addr) {
    pointer_arithmetic_t relative_addr = (pointer_arithmetic_t) addr;
    pointer_arithmetic_t memory_head = (pointer_arithmetic_t) memory_block_head;
    pointer_arithmetic_t size_of_block = (pointer_arithmetic_t) addr->size;
    relative_addr -= memory_head; // subtract off the start address of the block
    return (BlockHeader*) ((relative_addr ^ size_of_block) + memory_head); // get 0-based xor of the block's address and its size, then add head
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
    if(b1 < b2) { // whichever block is first, return it with double the size
        block1->size *= 2;
        BlockHeader* buddy = getbuddy(block1);
        if(free_list[free_list_index + 1].includes(buddy)) { // decided not to use free bit
            return merge(block1, buddy);
        }
        free_list[free_list_index + 1].insert(block1); // add big block in the next slot of free list
        return block1;
    }
    block2->size *= 2;
    BlockHeader* buddy = getbuddy(block2);
    if(free_list[free_list_index + 1].includes(buddy))
        return merge(block2, buddy);
    free_list[free_list_index + 1].insert(block2); // add big block in the next slot of free list
    return block2;
}

BlockHeader* BuddyAllocator::splitOnce (BlockHeader* block) {
    assert(block->size > basic_block_size);
    
    int free_list_index = getFreeListIndex(block->size);
    block->size /= 2; // update old block
    pointer_arithmetic_t block_ptr = (pointer_arithmetic_t) block; // will advance this to the buddy position
    block_ptr += (pointer_arithmetic_t) (block->size); // now at front of new buddy
    BlockHeader* new_buddy = (BlockHeader*) block_ptr;
    if(!free_list[free_list_index].empty()) // move old (now split) block to correct index in free list
        free_list[free_list_index].remove(block); // if the block doesn't exist in the list that's ok
    free_list[free_list_index - 1].insert(block);
    new_buddy->size = block->size;
    new_buddy->next = NULL;
    // do not add the new split block to the free list because it is about to be used
    return new_buddy;
}

BlockHeader* BuddyAllocator::split (BlockHeader* block, int size) {
    assert(block);
    int actual_block_size = block->size - BLOCKHEADER_SIZE;
    int next_smallest_block_size = block->size/2;
    if(next_smallest_block_size < basic_block_size)
        next_smallest_block_size = 0;
    else
        next_smallest_block_size -= BLOCKHEADER_SIZE;
    if(size > next_smallest_block_size && size <= actual_block_size) // if size is between next smallest and current, return the block
        return block;
    return split(splitOnce(block), size); // keep splitting until size is optimized
}

char* BuddyAllocator::getRawFromHeader(BlockHeader* block) {
    pointer_arithmetic_t raw = (pointer_arithmetic_t) block;
    raw += BLOCKHEADER_SIZE;
    return (char*) raw;
}
// advance the BlockHeader pointer to its raw memory block

BlockHeader* BuddyAllocator::getHeaderFromRaw(char* raw_block) {
    if(raw_block == NULL)
        return NULL;
    pointer_arithmetic_t header = (pointer_arithmetic_t) raw_block;
    header -= BLOCKHEADER_SIZE;
    // check to see if this memory is within bounds of what was allocated.
    if(memory_block_head > (void*)header) // use this structure as a bit of protection against garbage data
        return NULL;
    else if((void*)header > (void*)((pointer_arithmetic_t)memory_block_head + total_memory_length - ((BlockHeader*) header)->size))
        return NULL;
    return (BlockHeader*) header;
}

void BuddyAllocator::setBasicBlockSize(int input_size) { // assumes a power of 2 is entered
    basic_block_size = (uint) input_size;
}

void BuddyAllocator::setTotalMemoryLength(int input_size) { // assumes a power of 2 is entered
    total_memory_length = (uint) input_size;
}

int BuddyAllocator::getFreeListIndex(int requested_size) {
    double argument = (requested_size)/(double)(basic_block_size);
    if(argument < 1) // domain check for log2(a)
        argument = 1;
    return (int) ceil(log2(argument));
}

char* BuddyAllocator::alloc(int _length) {
    // go to smallest slot possible in free list and search until a free block is found
    int smallest_index = getFreeListIndex(_length + BLOCKHEADER_SIZE);
    if(!free_list[smallest_index].empty()) { // if the first slot available has some free
        BlockHeader* block = free_list[smallest_index].back();
        if(!free_list[smallest_index].empty())
            free_list[smallest_index].remove(block);
        // return raw block
        return getRawFromHeader(block);
    }
    for(int i = smallest_index + 1; i <= largest_block_index; i++) {
        if(!free_list[i].empty()) {
            BlockHeader* block = free_list[i].back();
            if(!free_list[i].empty())
                free_list[i].remove(block);
            // recursively split block
            // return raw block
            return getRawFromHeader(split(block, _length));
        }
    }
    return NULL;
}

int BuddyAllocator::free(char* _a) {
    // get the blockheader, add it to the free list or merge with its buddy in the free list
    BlockHeader* block = getHeaderFromRaw(_a);
    if(!block) {
        return -1;
    }
    BlockHeader* buddy = getbuddy(block);
    int free_list_index = getFreeListIndex(block->size);
    if(free_list[free_list_index].includes(buddy)) {
        merge(block, buddy);
    } else {
        free_list[free_list_index].insert(block);
    }
    return 0;
}

// DEBUG FUNCTIONS

void BuddyAllocator::printFreeListState() {
    for(int i = 0; i <= largest_block_index; i++) {
        cout << ":i = " << i << ": ";
        if(!free_list[i].empty()) {
            BlockHeader* curr = free_list[i].front();
            cout << (void*)((pointer_arithmetic_t)curr - (pointer_arithmetic_t)memory_block_head);
            curr = curr->next;
            while(curr) {
                cout << " - " << (void*)((pointer_arithmetic_t)curr - (pointer_arithmetic_t)memory_block_head);
                curr = curr->next;
            }
        }
        cout << endl;
    }
}

void BuddyAllocator::splitOnceTest() {
    BlockHeader* b = free_list[largest_block_index].back();
    BlockHeader* c = splitOnce(b);
    cout << "split block which was not added to free list: " << (void*)((pointer_arithmetic_t)c - (pointer_arithmetic_t)memory_block_head) << endl;
    printFreeListState();
    cout << "split again." << endl;
    BlockHeader* d = splitOnce(c);
    cout << "split block which was not added to free list: " << (void*)((pointer_arithmetic_t)d - (pointer_arithmetic_t)memory_block_head) << endl;
    printFreeListState();
}

void BuddyAllocator::splitTest() {
    BlockHeader* b = free_list[largest_block_index].back();
    BlockHeader* c = split(b, 497);
    void* c_check = (void*)((pointer_arithmetic_t)c - (pointer_arithmetic_t)memory_block_head);
    assert(c_check == (void*)0x0);
    BlockHeader* d = split(c, 496);
    void* d_check = (void*)((pointer_arithmetic_t)d - (pointer_arithmetic_t)memory_block_head);
    assert(d_check == (void*)0x200);
    BlockHeader* e = split(d, 205);
    void* e_check = (void*)((pointer_arithmetic_t)e - (pointer_arithmetic_t)memory_block_head);
    assert(e_check == (void*)0x300);
    BlockHeader* f = split(e, 1);
    void* f_check = (void*)((pointer_arithmetic_t)f - (pointer_arithmetic_t)memory_block_head);
    assert(f_check == (void*)0x380);
//    printFreeListState();
    cout << "Split Test Finished." << endl;
}

void BuddyAllocator::allocTest() {
    std::cout << "simple alloc test -- ";
    char* a = alloc(100);
    void* a_test = (void*)((pointer_arithmetic_t)a - (pointer_arithmetic_t)memory_block_head);
    assert(a_test == (void*)0x390);
//    printFreeListState();
    char* b = alloc(4);
    void* b_test = (void*)((pointer_arithmetic_t)b - (pointer_arithmetic_t)memory_block_head);
    assert(b_test == (void*)0x310);
//    printFreeListState();
    char* c = alloc(466);
    void* c_test = (void*)((pointer_arithmetic_t)c - (pointer_arithmetic_t)memory_block_head);
    assert(c_test == (void*)0x10);
//    printFreeListState();
    char* d = alloc(100);
    void* d_test = (void*)((pointer_arithmetic_t)d - (pointer_arithmetic_t)memory_block_head);
    assert(d_test == (void*)0x290);
//    printFreeListState();
    char* e = alloc(112);
    void* e_test = (void*)((pointer_arithmetic_t)e - (pointer_arithmetic_t)memory_block_head);
    assert(e_test == (void*)0x210);
//    printFreeListState();
    cout << "PASSED" << endl;
}

void BuddyAllocator::freeTest() { // assumes allocTest() passes
    std::cout << "simple free test -- ";
    char* a = alloc(100);
    char* b = alloc(4);
    char* c = alloc(466);
    char* d = alloc(100);
    char* e = alloc(112);
    free(a);
//    printFreeListState();
    free(e);
//    printFreeListState();
    free(b);
//    printFreeListState();
    free(d);
//    printFreeListState();
    free(c);
//    printFreeListState();
    cout << "DONE." << endl;
}

void BuddyAllocator::debug () { // assumes a total memory size of 1024 and basic block of 128
    cout << "Test Battery:" << endl;
//    splitTest();
//    allocTest();
    freeTest();
}
