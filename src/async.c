#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "common.h"

#include "async.h"

static void handle_write(int signal, siginfo_t *siginfo, void *context) {
    if (SIGIO == signal) {
        //redirect_input(0, pfd[0][1], log_fd);
        fprintf(stderr, "Is not implemented.\n");
    } else if (SIGUSR1 == signal) {
        redirect_output(pfd[1][0], 1, log_fd);
    } else if (SIGUSR2 == signal) {
        redirect_output(pfd[2][0], 2, log_fd);
    } else {
        fprintf(stderr, "Unexpected signal %d.\n", signal);
        exit(EXIT_FAILURE);
    }
}

void async(const char *logfile, char *command) {
    create_handler(SIGCHLD, &handle_child);
    create_handler(SIGUSR1, &handle_write);
    create_handler(SIGUSR2, &handle_write);
    create_handler(SIGIO, &handle_write);
    init_pipes();

    pid_t pid = fork();

    log_fd = my_file_open(logfile);
    loop = true;

    if (0 == pid) {
        async_child(command);
    } else if (pid > 0) {
        async_parent();
    } else {
        fprintf(stderr, "%10d Pid is negative after fork. %s\n", getpid(), strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void async_child(char *command) {
    // TODO: redirect input here
    EXIT_ON_FAILURE(dup2(pfd[1][1], 1));
    EXIT_ON_FAILURE(fcntl(pfd[1][0], F_SETOWN, getppid()));
    EXIT_ON_FAILURE(fcntl(pfd[1][0], F_SETFL, O_ASYNC | O_NONBLOCK));
    EXIT_ON_FAILURE(fcntl(pfd[1][0], F_SETSIG, SIGUSR1));
    EXIT_ON_FAILURE(dup2(pfd[2][1], 2));
    EXIT_ON_FAILURE(fcntl(pfd[2][0], F_SETOWN, getppid()));
    EXIT_ON_FAILURE(fcntl(pfd[2][0], F_SETFL, O_ASYNC | O_NONBLOCK));
    EXIT_ON_FAILURE(fcntl(pfd[2][0], F_SETSIG, SIGUSR2));
    my_execute(command);
}

void async_parent() {
    while (loop) {
        sleep(1);
    }

    fprintf(stderr, "%10d Terminated with exit code: %d\n", getpid(), child_exit_code);
}
