#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "common.h"

void my_execute(char *command) {
    char **args = (char **)malloc(MAX_ARGS * sizeof(char *));
    char *pch = strtok(command, " ");
    args[0] = pch;
    int i = 0;

    while (pch != NULL) {
        pch = strtok(NULL, " ");
        args[++i] = pch;
    }  // TODO: check index out of array

    execvp(*args, args);
    fprintf(stderr, "%10d Can not execute \"%s\". %s.\n", getpid(), *args, strerror(errno));
    exit(EXIT_FAILURE);
}

void write_buffer(const int fd_num, char *buffer) {
    static char *fd_name[] = { ">0", "<1", "<2" };

    char *pch = strtok(buffer, "\n");

    while (pch != NULL) {
        fprintf(stdout, "%10d %s %s\n", getpid(), fd_name[fd_num], pch);
        pch = strtok(NULL, "\n");  // TODO: next line
    }
}

void write_time(const int log_fd) {
    char time_str[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(time_str, 9, "%H:%M:%S", &tm);
    write(log_fd, time_str, 9);
}

void write_noio(const int log_fd) {
    write_time(log_fd);
    write(log_fd, ", NOIO\n", 7);
}

void redirect_output(int read_fd, int write_fd, int log_fd) {
    for (;;) {
        int count = read(read_fd, buffer, READ_BUFFER_SIZE);

        if (-1 == count) {
            if (EAGAIN == errno) { break; }
            if (EINTR == errno) { continue; }
            perror("read. ");
            exit(EXIT_FAILURE);
        }

        if (0 == count) { break; }

        // (count > 0)
        buffer[count] = 0;

        if (log_fd != FD_NULL) {
            write_time(log_fd);
            write(log_fd, ", ", 2);
            write(log_fd, buffer, count);
        }

        write_buffer(write_fd, buffer);
    }
}

void redirect_input(int read_fd, int write_fd, int log_fd) {
    for (;;) {
        int count = read(read_fd, buffer, READ_BUFFER_SIZE);

        if (-1 == count) {
            if (EAGAIN == errno) { break; }
            if (EINTR == errno) { continue; }
            perror("Read. ");
            exit(EXIT_FAILURE);
        }

        // (count >= 0)
        buffer[count] = '\0';

        if (log_fd != FD_NULL) {
            write_time(log_fd);
            write(log_fd, ", ", 2);
            write(log_fd, buffer, count);
        }

        write_buffer(0, buffer);
        write(write_fd, buffer, count - 1);
        write(write_fd, "\n", 1);
        break;
    }
}

int my_file_open(const char *name) {
    int fd;

    if (*name == '\0') {
        return FD_NULL;
    }

    fd = open(name, O_CREAT | O_TRUNC | O_WRONLY | O_NONBLOCK, S_IRUSR | S_IWUSR);

    if (-1 == fd) {
        perror("Can not open logfile.");
        return FD_NULL;
    }

    return fd;
}

// TODO: looks like class constructor
void init_pipes() {
    int i;

    for (i = 0; i < 3; i++) {
        if (-1 == pipe(pfd[i])) {
            fprintf(stderr, "%10d Error while pipe. %s\n", getpid(), strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    child_exit_code = -1;
    log_fd = FD_NULL;
}

void create_handler(int signum, function_handler_t sigactionvalue) {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);
    sa.sa_sigaction = sigactionvalue;

    if (-1 == sigaction(signum, &sa, NULL)) {
        fprintf(stderr, "%10d Fail while creating handler. %s\n", getpid(), strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void handle_child(int signal, siginfo_t *siginfo, void *context) {
    child_exit_code = signal;
    loop = false;
}
