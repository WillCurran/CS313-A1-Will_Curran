/* 
    File: my_allocator.h

    Original Author: R.Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified:

 */

#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#define MAX_MEM_INDEX 64
#define BLOCKHEADER_SIZE 16 // sizeof(some_BlockHeader) = 16

#include <iostream>
#include <assert.h>
using namespace std;
typedef uintptr_t pointer_arithmetic_t;

class BlockHeader {
public:
    uint size; // how big is the entire block?
    BlockHeader* next;
};

class LinkedList {
	// this is a special linked list that is made out of BlockHeader s. 
private:
    BlockHeader* head;        // you need a head of the list
public:
    LinkedList() {
        head = NULL;
    }
    
	void insert (BlockHeader* b) {	// adds a block to the list
        if(head) {
            BlockHeader* curr = head;
            while(curr->next)
                curr = curr->next;
            curr->next = b;
        } else {
            head = b;
        }
	}

	void remove (BlockHeader* b) {  // removes a block from the list if it exists, but does not delete it
        assert(head);
        if(head == b) {
            head = NULL;
            return;
        }
        BlockHeader* curr = head;
        while(curr->next) {
            if(curr->next == b) {
                curr->next = curr->next->next;
                return;
            }
        }
	}
    
    bool empty() {
//        cout << head << endl;
        return head == NULL;
    }
    
    bool includes(BlockHeader* b) { // check if this element exists in the list
        assert(head);
        if(head == b) {
            return true;
        }
        BlockHeader* curr = head;
        while(curr->next) {
            if(curr->next == b) {
                return true;
            }
        }
        return false;
    }
    
    BlockHeader* back() {
        // get the last element in the list
        if(!head) {
            return NULL;
        }
        BlockHeader* curr = head;
        while(curr->next) {
            curr = curr->next;
        }
        return curr;
    }
};


class BuddyAllocator {
private:
    uint basic_block_size;
    uint total_memory_length;
    int largest_block_index;
    void* memory_block_head;
    LinkedList free_list[MAX_MEM_INDEX];
    
private:
	BlockHeader* getbuddy (BlockHeader * addr); 
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2);
	// checks whether the two blocks are buddies are not

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around
    // assumes blocks are of equal size

	BlockHeader* splitOnce (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected
    
    BlockHeader* split (BlockHeader* block, int block_size);
    // recursively splits block until maximized for the given size
    
    char* getRawFromHeader(BlockHeader* block);
    // advance the BlockHeader pointer to its raw memory block
    
    BlockHeader* getHeaderFromRaw(char* raw_block);
    // backtrack from raw memory block pointer to its BlockHeader
    
    void setBasicBlockSize(int input_size);
    void setTotalMemoryLength(int input_size);
    int getFreeListIndex(int requested_size);
    // get an index corresponding to a slot in the free_list which could offer enough memory for the user
    
    // DEBUG FUNCTIONS
    
    void splitOnceTest();
    void splitTest();
    void allocTest();
    

public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. If an error occurred, 
	   it returns 0. 
	*/ 

	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok. */
   
	void debug ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	128: 5
	256: 0
	512: 3
	1024: 0
	....
	....
	 which means that at point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
    
    void printFreeListState();
};

#endif 
