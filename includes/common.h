#ifndef __COMMON__
#define __COMMON__

#define MAX_ARGS 8
#define READ_BUFFER_SIZE 512
#define FD_NULL -1

#define PIN pfd[0]
#define POUT pfd[1]
#define PERR pfd[2]
#define PIPE_WRITE 0
#define PIPE_READ 1

void my_execute(char*);
void write_buffer(const int fd_num, char * buffer);
void write_noio(const int log_fd);
void read_avaible(const int read_fd, const int write_fd, const int log_fd);
int my_file_open(const char * name);
void init_pipes();

char loop;
int chld_exit_code;
int pfd[3][2];
char buffer[READ_BUFFER_SIZE + 1];
int log_fd;

#endif
