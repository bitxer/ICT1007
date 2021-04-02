#ifndef _PAPER1_H
#define _PAPER1_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#define _DEBUG
#ifdef _DEBUG
    #define DEBUG(...) fprintf(stderr, __VA_ARGS__);
#else
    #define DEBUG(...)
#endif

// Create mutex
pthread_mutex_t mutex;

struct Arguments {
    int thread_id;
    char * src;
    char * dest;
    int t_execution;
};

typedef struct Arguments ARGS;
typedef ARGS *ARGS_PTR;

void * copy(void * _args);
int main(int argc, char * argv[]);
#endif