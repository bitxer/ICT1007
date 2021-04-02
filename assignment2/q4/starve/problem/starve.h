#ifndef _PAPER1_H
#define _PAPER1_H

// Included header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <libgen.h>
#include <sys/stat.h>

// Create mutex
pthread_mutex_t mutex;

// Struct to hold arguments passed to thread
// It also holds some return values
struct Arguments {
    int thread_id;
    char * src;
    char * dest;
    int t_execution;
};

typedef struct Arguments ARGS;
typedef ARGS *ARGS_PTR;

// Function proto types
void * copy(void * _args);
int main(int argc, char * argv[]);
#endif