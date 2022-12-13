#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "async.h"
#include "select.h"

struct App {
    char *logfile;
    char *command;
    bool multiplex;
};

struct App *parse_arguments(int argc, char **argv) {
    int digit_optind = 0;

    struct App *app = (struct App *)malloc(sizeof(struct App));

    app->logfile = "";
    app->command = "";
    app->multiplex = true;

    for (;;) {
        int option_index = 0;

        static struct option long_options[] = {
            { "logfile",   1, 0, 0 },  // index = 0
            { "execute",   1, 0, 0 },  // index = 1
            { "multiplex", 1, 0, 0 },  // index = 2
            { "select",    0, 0, 0 },  // index = 3
            { "async",     0, 0, 0 },  // index = 4
            { 0,           0, 0, 0 }
        };

        char c = getopt_long(argc, argv, "", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 0:
                // parse --long arg
                switch (option_index) {
                    case 0:  // logfile
                        app->logfile = optarg;
                        break;
                    case 1:  // execute
                        app->command = optarg;
                        break;
                    case 2:  // multiplex
                        app->multiplex = atoi(optarg);
                        break;
                    case 3:  // select
                        app->multiplex = true;
                        break;
                    case 4:  // async
                        app->multiplex = false;
                        break;
                }
                break;
            case '?':
                // parse short arg
            default:
                fprintf(stderr, "Error while parsing args.\n");
                exit(EXIT_FAILURE);
        }
    }

    return app;
}

int main(int argc, char **argv) {
    srand(time(NULL));

    struct App *app = parse_arguments(argc, argv);

    if (app->multiplex) {
        printf("%s\n", "Run process_select(logfile, command)");
        process_select(app->logfile, app->command);
        return EXIT_SUCCESS;
    }

    printf("%s\n", "Run async(logfile, command)");
    async(app->logfile, app->command);
    return EXIT_SUCCESS;
}

