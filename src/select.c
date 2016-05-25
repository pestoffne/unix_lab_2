#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "common.h"
#include "select.h"

static void handle_child(int signal, siginfo_t *siginfo, void *context) {
    chld_exit_code = signal;
    loop = 0;
}

void process_select(char * logfile, char * command) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigset_t mask;
    sigfillset(&mask);
    sa.sa_mask = mask;
    sa.sa_sigaction = &handle_child;
    if (-1 == sigaction(SIGCHLD, &sa, NULL)) {
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(2);
    }

    init_pipes();

    int log_fd = my_file_open(logfile);
    loop = 1;

    pid_t pid = fork();
    if (0 == pid) { // child
        if (-1 == dup2(PIN[PIPE_WRITE], 0)) {
            perror("Error while dup2(pfd[0][0], 0). ");
            exit(2);
        }
        if (-1 == dup2(POUT[PIPE_READ], 1)) {
            perror("Error while dup2(pfd[1][1], 1). ");
            exit(2);
        }
        if (-1 == dup2(PERR[PIPE_READ], 2)) {
            perror("Error while dup2(pfd[2][1], 2). ");
            exit(2);
        }
        my_execute(command);
    } else if (pid > 0) { // parent

        fcntl(POUT[PIPE_WRITE], F_SETFL, O_NONBLOCK);
        fcntl(PERR[PIPE_WRITE], F_SETFL, O_NONBLOCK);

        do {
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;

            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(0, &readfds);
            FD_SET(pfd[1][0], &readfds);
            FD_SET(pfd[2][0], &readfds);

            int retval = select(9, &readfds, NULL, NULL, &tv);
            if (-1 == retval) {
                if (EINTR == errno) {
                    // do nothing
                } else {
                    perror("select. ");
                    exit(2);
                }
            } else if (0 == retval) {
                if (log_fd == FD_NULL) {
                    write_noio(2);
                } else {
                    write_noio(log_fd);
                }
            } else {
                if (FD_ISSET(0, &readfds)) {
                    redirect_input(0, pfd[0][1], log_fd);
                }
                if (FD_ISSET(POUT[PIPE_WRITE], &readfds)) {
                    redirect_output(pfd[1][0], 1, log_fd);
                }
                if (FD_ISSET(PERR[PIPE_WRITE], &readfds)) {
                    redirect_output(pfd[2][0], 2, log_fd);
                }
            }
        } while (loop);

        fprintf(stderr, "%10d TERMINATED WITH EXIT CODE: %d\n", getpid(), chld_exit_code);
    } else { // (-1 == pid) error
        fprintf(stderr, "%10d Pid is negative, after fork. %s\n", getpid(), strerror(errno));
        exit(2);
    }
}
