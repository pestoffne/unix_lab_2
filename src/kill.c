#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "kill.h"

// uses global: pid, signo
void script_kill() {
    if (-1 == kill(pid, signo)) {
        fprintf(stderr, "kill(%i, %d)", pid, signo);
        perror("Fail while sending signal");
        exit(12);
    }
}
