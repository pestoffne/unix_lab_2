#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handle_pipe(int signal, siginfo_t *siginfo, void *context) {
    if (SIGPIPE == signal) {
        printf("Handler: SIGPIPE catched.\n");
    } else {
        fprintf(stderr, "Handler: Unexpected signal.\n");
    }
}

void script_pipe() {
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &handle_pipe;
    if (-1 == sigaction(SIGPIPE, &sa, NULL)) {
        perror("?:       Fail while creating handler.");
        exit(17);
    }

    // open pipe
    int pipefd[2];
    if (-1 == pipe(pipefd)) {
        perror("?:       Fail while openning pipe.");
        exit(13);
    }

    pid_t pid = fork();
    if (0 == pid) {  // child
        printf("Child:   Started.\n");
        printf("Child:   Finished.\n");
    } else if (pid > 0) {  // parent
        printf("Parent:  Started.\n");
        // close read fd
        if (-1 == close(pipefd[0])) {
            perror("Parent:  Can not close read fd of pipe.");
            exit(14);
        }
        sleep(1);
        write(pipefd[1], "foo", 12);
        printf("Parent:  Finished.\n");
    } else {  // (-1 == pid) error
        perror("?:       Pid is negative, after fork.\n");
        exit(10);
    }
}
