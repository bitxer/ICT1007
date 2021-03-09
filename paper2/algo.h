#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define __DEBUG__
#ifdef __DEBUG__
#define DBG(...) printf(__VA_ARGS__);
#else
#define DBG(...) ;
#endif

typedef struct Process {
    int process_id;
    int arrival_time;
    int burst_time;
    int turnaround_time;
    int waiting_time;
    struct Process* next;
} ProcessList;

int DEBUG_MODE = 0;
int K_INCREMENT = 0;
int BURST_TIME = 1;
int ARRIVAL_TIME = 2;
int APPEND = 0;

void printHelp(const char* name, const char* option);
int getTestData(char* filepath, int* num_of_processes, ProcessList** process_list, ProcessList** ready_queue, ProcessList** incoming_arrivals) ;
void sortedInsert(ProcessList** head, ProcessList* node, int sortBy, ProcessList* protectedProcess);
ProcessList* createProcess(int process_id, int arrival_time, int burst_time, int waiting_time, int turnaround_time);
void printProcessList(ProcessList*  n);
ProcessList* runProcesses(ProcessList* ready_queue, ProcessList* incoming_arrivals, int* time, int* k_factor, int* total_num_of_processes);
ProcessList* copyProcess(ProcessList* source);
void checkArrivals(ProcessList** ready_queue, ProcessList** incoming_arrivals, int* time, ProcessList* protectedProcess);
void calculateStats(ProcessList* process_list);