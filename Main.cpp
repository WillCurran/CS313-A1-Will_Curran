#include "Ackerman.h"
#include "BuddyAllocator.h"
#include "getopt.h"
#include <unistd.h>

int main(int argc, char ** argv) {

    int basic_block_size = 128, memory_length = 64 * 1024 * 1024;
    
    int option;
    int errflag = 0;
    extern char *optarg;
    extern int optind, optopt;
    
    for(int i = 0; i < argc; i++) {
        cout << argv[i] << endl;
    }

    while ((option = getopt(argc, argv, "b:s:")) != -1) {
        switch(option) {
            case 'b':
                if(isdigit(optarg[0]))
                    basic_block_size = atoi(optarg);
                else
                    errflag++;
                break;
            case 's':
                if(isdigit(optarg[0]))
                    memory_length = atoi(optarg);
                else
                    errflag++;
                break;
            case '?':
                errflag++;
        }
    }
    if (errflag) {
        fprintf(stderr, "usage: memtest [-b <blocksize>] [-s <memsize>]\n");
        return 1;
    }
    
    
    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);
//    allocator->debug();
    // test memory manager
    Ackerman* am = new Ackerman ();
    am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
    delete allocator;
}
