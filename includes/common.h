#ifndef __COMMON__
#define __COMMON__

#define MAX_ARGS 8
#define READ_BUFFER_SIZE 512

char buffer[READ_BUFFER_SIZE + 1];

void my_execute(char*);
void write_buffer(int, char*);
void write_noio();

#endif
