#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "common.h"
#include "select.h"

void process_select(const char *logfile, char *command) {
    create_handler(SIGCHLD, &handle_child);

    init_pipes();

    int log_fd = my_file_open(logfile);
    loop = true;

    pid_t pid = fork();

    if (0 == pid) {
        process_select_child(command);
    } else if (pid > 0) {
        process_select_parent();
    } else {  // (-1 == pid)
        fprintf(stderr, "%10d Pid is negative, after fork. %s\n", getpid(), strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void process_select_child(char *command) {
    EXIT_ON_FAILURE(dup2(pfd[0][0], 0));
    EXIT_ON_FAILURE(dup2(pfd[1][1], 1));
    EXIT_ON_FAILURE(dup2(pfd[2][1], 2));
    my_execute(command);
}

void process_select_parent() {
    EXIT_ON_FAILURE(fcntl(pfd[1][0], F_SETFL, O_NONBLOCK));
    EXIT_ON_FAILURE(fcntl(pfd[2][0], F_SETFL, O_NONBLOCK));

    do {
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(pfd[1][0], &readfds);
        FD_SET(pfd[2][0], &readfds);

        int ret = select(9, &readfds, NULL, NULL, &tv);

        if (-1 == ret) {
            if (EINTR == errno) {
                // do nothing
            } else {
                perror("select. ");
                exit(EXIT_FAILURE);
            }
        } else if (0 == ret) {
            write_noio(log_fd);
            //write_noio(2);
        } else {
            if (FD_ISSET(0, &readfds)) {
                redirect_input(0, pfd[0][1], log_fd);
            }
            if (FD_ISSET(pfd[1][0], &readfds)) {
                redirect_output(pfd[1][0], 1, log_fd);
            }
            if (FD_ISSET(pfd[2][0], &readfds)) {
                redirect_output(pfd[2][0], 2, log_fd);
            }
        }
    } while (loop);

    fprintf(stderr, "%10d Terminated with exit code: %d\n", getpid(), child_exit_code);
}
