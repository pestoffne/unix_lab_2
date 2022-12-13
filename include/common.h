#pragma once

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_ARGS 8
#define READ_BUFFER_SIZE 512
#define FD_NULL -1

void my_execute(char *);
void write_buffer(const int, char *);
void write_noio(const int);
void read_available(const int, const int, const int);
int my_file_open(const char *);
void init_pipes();

typedef void (*function_handler_t)(int, siginfo_t *, void *);
void handle_child(int, siginfo_t *, void *);
void create_handler(int, function_handler_t);

void redirect_output(int, int, int);
void redirect_input(int, int, int);

// TODO: looks like class fields
bool loop;
int child_exit_code;
int pfd[3][2];
char buffer[READ_BUFFER_SIZE + 1];
int log_fd;

#define EXIT_ON_FAILURE(ret)                                                  \
    if (-1 == ret) {                                                          \
        fprintf(stderr, "%10d %s:%d %s\n",                                    \
            getpid(), __FILE__, __LINE__, strerror(errno));                   \
        exit(EXIT_FAILURE);                                                   \
    }
