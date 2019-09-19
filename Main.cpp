#include "Ackerman.h"
#include "BuddyAllocator.h"
#include "getopt.h"

int main(int argc, char ** argv) {

  int basic_block_size = 128, memory_length = 512 * 1024;
    
    int option;
//    int bflg, aflg, errflg;
//    char *ifile;
//    char *ofile;
    extern char *optarg;
    extern int optind, optopt;

    while ((option = getopt(argc, argv, ":bs:")) != -1) {
        switch(option) {
            case 'b':
//                basic_block_size = optopt;
                cout << optarg << endl;
//                if (bflg)
//                    errflg++;
//                else
//                    aflg++;
                break;
            case 's':
//                memory_length = optopt;
                cout << optarg << endl;
//                if (aflg)
//                    errflg++;
//                else {
//                    bflg++;
//                    bproc();
//                }
                break;
            case ':':       /* -f or -o without operand */
                fprintf(stderr,
                        "Option -%c requires an operand\n", optopt);
//                errflg++;
                break;
            case '?':
                fprintf(stderr,
                        "Unrecognized option: -%c\n", optopt);
//                errflg++;
        }
    }
//    if (errflg) {
//        fprintf(stderr, "usage: . . . ");
//        exit(2);
//    }
//    for ( ; optind < argc; optind++) {
//        if (access(argv[optind], R_OK)) {
//            . . .
    
    
    
//    BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);
//    allocator->debug();
    // test memory manager
//     Ackerman* am = new Ackerman ();
//     am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
//    delete allocator;
}
