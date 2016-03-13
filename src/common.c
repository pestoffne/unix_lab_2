#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "child.h"

// modes: std, posix, kill, pipe, child
script_t get_script(char * name) {
    if (strcmp(name, "child") == 0) {
        printf("child");
        return &script_child;
    } else {
        perror("unexpected mode value");
        exit(4);
    }
}

script_t parse_args(int argc, char ** argv) {
    // prints all args
    //printf("args list:\r\n");
    //int i;
    //for (i = 0; i < argc; i++) {
    //    printf("arg[%d] = %s\r\n", i, *(argv + i));
    //}
    // parse using getopt
    int c;
    int digit_optind = 0;
    script_t script;

    while (1) {
        // int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"mode",   1, 0, 0 }, // index = 0
            {"amount", 1, 0, 0 }, // index = 1
            {"signo",  1, 0, 0 }, // index = 2
            {"pid",    1, 0, 0 }, // index = 3
            {0,        0, 0, 0 }
        };

        c = getopt_long(argc, argv, "", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                // parse --long arg
                switch (option_index) {
                    case 0: // mode
                        printf("mode ");
                        script = get_script(optarg);
                        break;
                    case 1: // amount
                        printf("amount ");
                        break;
                    case 2: // signo
                        printf("signo ");
                        break;
                    case 3: // pid
                        printf("pid ");
                        break;
                }
                //printf("option %s", long_options[option_index].name);
                //if (optarg)
                //    printf(" with arg %s", optarg);
                printf("\n");
                break;
            case '?':
                perror("unexpected argument\r\n");
                exit(2);
            default:
                perror("error while parseing args\r\n");
                exit(3);
        }
    }
    return script;
}
