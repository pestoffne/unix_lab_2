#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "child.h"
#include "std.h"
#include "kill.h"
#include "pipe.h"

typedef void (*script_t)();

void default_script() {
    fprintf(stderr, "Mode is not selected. Nothing to do.\n");
}

script_t get_script(char * name) {
    // Gets mode name and returns function that contains the corresponding script
    // modes: std, posix, kill, pipe, child
    if (0 == strcmp(name, "child")) {
        return &script_child;
    } else if (0 == strcmp(name, "std")) {
        return &script_std;
    } else if (0 == strcmp(name, "kill")) {
        return &script_kill;
    } else if (0 == strcmp(name, "pipe")) {
        return &script_pipe;
    } else {
        fprintf(stderr, "Unexpected mode value.\n");
        exit(4);
    }
}

int main(int argc, char ** argv) {
    // parse using getopt
    int c;
    int digit_optind = 0;
    script_t script = &default_script;

    while (1) {
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
                    case 0:  // mode
                        script = get_script(optarg);
                        break;
                    case 1:  // amount
                        // amount = optarg TODO
                        break;
                    case 2:  // signo
                        signo = atoi(optarg);
                        break;
                    case 3:  // pid
                        pid = atoi(optarg);
                        break;
                }
                break;
            case '?':
                // parse short arg
                exit(2);
            default:
                fprintf(stderr, "Error while parseing args.\n");
                exit(3);
        }
    }
    script();
    return 0;
}
