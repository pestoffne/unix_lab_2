#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "common.h"

void my_execute(char * command) {
    char ** args = (char**)malloc(MAX_ARGS * sizeof(char*));
    char * pch = strtok(command, " ");
    args[0] = pch;
    int i = 0;
    while (pch != NULL) {
        pch = strtok(NULL, " ");
        args[++i] = pch;
    } // TODO checks index out of array
    execvp(*args, args);
    fprintf(stderr, "%10d Can not execute \"%s\". %s.\n", getpid(), *args, strerror(errno));
    exit(2);
}

void write_buffer(const int fd_num, char * buffer) {
    static char* fd_name[] = {">0", "<1", "<2"
        ,"a", "b", "c", "d", "e", "f", "g" // DEBUG
        };
    char * pch = strtok(buffer, "\n");
    while (pch != NULL) {
        fprintf(stdout, "%10d %s %s\n", getpid(), fd_name[fd_num], pch);
        pch = strtok(NULL, "\n"); // TODO next line
    }
}

void write_noio(const int log_fd) {
    char time_str[9];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(time_str, 9, "%H:%M:%S", &tm);
    write(log_fd, time_str, 9);
    write(log_fd, ", NOIO\n", 7); // TODO use write once
}

void read_avaible(const int read_fd, const int write_fd, const int log_fd) {
    while (1) {
        int count = read(read_fd, buffer, READ_BUFFER_SIZE);
        if (-1 == count) {
            if (EAGAIN == errno) {
                break;
            } else if (EINTR == errno) {
                // do nothing
            } else {
                perror("read. ");
                exit(2);
            }
        } else if (0 == count) {
            break;
        } else { // (count > 0)
            buffer[count] = 0;
            if (log_fd != FD_NULL) {
                write(log_fd, buffer, count);
            }
            write_buffer(write_fd, buffer);
        }
    }
}

void read_avaible_c(const int read_fd, const int write_fd, const int log_fd) {
    while (1) {
        int count = read(read_fd, buffer, READ_BUFFER_SIZE);
        if (-1 == count) {
            if (EAGAIN == errno) {
                break;
            } else if (EINTR == errno) {
                // do nothing
            } else {
                perror("Read. ");
                exit(2);
            }
        } else { // (count > 0)
            buffer[count] = 0;
            if (log_fd != FD_NULL) {
                write(log_fd, buffer, count);
            }
            write_buffer(0, buffer);
            write(write_fd, buffer, count);
            write(write_fd, "\n", 1);
            break;
        }
    }
}

int my_file_open(const char * name) {
    int fd;
    if (*name == '\0') {
        return FD_NULL;
    } else {
        fd = open(name, O_CREAT | O_WRONLY | O_NONBLOCK, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("Can not open logfile.");
            return FD_NULL;
        }
        return fd;
    }
}

void init_pipes() {
    int i;
    for (i = 0; i < 3; i++) {
        if (-1 == pipe(pfd[i])) {
            fprintf(stderr, "%10d Error while pipe. %s\n", getpid(), strerror(errno));
            exit(2);
        }
    }
    chld_exit_code = -1;
    log_fd = FD_NULL;
}
