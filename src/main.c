#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "async.h"
#include "select.h"

int main(int argc, char ** argv) {
    srand(time(NULL));
    // parse using getopt
    int c;
    int digit_optind = 0;

    char * logfile = "";
    char * command = "";
    char multiplex = 1;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"logfile",   1, 0, 0 }, // index = 0
            {"execute",   1, 0, 0 }, // index = 1
            {"multiplex", 1, 0, 0 }, // index = 2
            {0,           0, 0, 0 }
        };

        c = getopt_long(argc, argv, "", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                // parse --long arg
                switch (option_index) {
                    case 0:  // logfile
                        logfile = optarg;
                        break;
                    case 1:  // execute
                        command = optarg;
                        break;
                    case 2:  // multiplex
                        multiplex = atoi(optarg);
                }
                break;
            case '?':
                // parse short arg
                exit(2);
            default:
                fprintf(stderr, "Error while parseing args.\n");
                exit(2);
        }
    }
    
    if (0 == multiplex) {
        async(logfile, command);
    } else if (1 == multiplex) {
        process_select(logfile, command);
    } else {
        fprintf(stderr, "Incorrect multiplex value : '%s'.\n", optarg);
        exit(2);
    }
    return 0;
}
