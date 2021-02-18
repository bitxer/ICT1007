#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

void printHelp(const char* name, const char* option);
void sortedInsert(ProcessList** head, ProcessList* node);
ProcessList* createProcess(int process_id, int arrival_time, int burst_time, int waiting_time, int turnaround_time);
ProcessList* getTestData();
void printProcessList(ProcessList*  n);
void runProcesses(ProcessList* processes, int* time, int* k_factor);
