#ifndef _PAPER1_H
#define _PAPER1_H
// Global Imports
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// Development Functions
// #define _DEBUG
// #ifdef _DEBUG
//     #define DEBUG(...) fprintf(stderr, __VA_ARGS__);
// #else
//     #define DEBUG(...)
// #endif


extern int QUIET;
extern int INTERACTIVE;
#define TRUE 1
#define FALSE 0

#define VERBOSE(...) ({                 \
    if (!(QUIET)) {                     \
        (fprintf)(stdout, __VA_ARGS__); \
    }                                   \
})

#define PROMPT(...) ({                  \
    if (INTERACTIVE) {                  \
        (fprintf)(stdout, __VA_ARGS__); \
    }                                   \
})

// Return codes for insert_process_q()
#define SUCCESS 0
#define UNEXPECTED_ARR_TIME 1

// Status code for status_flag in Process
#define NEW 0
#define READY 1
#define TERMINATED 2

// Data structure for Process
struct Process {
    int pid;
    int t_arrival;
    int t_exec;
    int t_remain;
    int t_turn;
    int t_wait;
    int status_flag;
    struct Process *next;
};

typedef struct Process PROCESS;
typedef PROCESS *PROCESS_PTR;

// Function prototypes
void sort_ready_queue();
void print_queue(PROCESS_PTR q_head);
PROCESS_PTR check_arrival(int t_current);
void insert_ready_q(PROCESS_PTR arrival_q);
int insert_process_q(int pid, int t_arrival, int t_exec);
#endif