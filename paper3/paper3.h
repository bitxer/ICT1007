#include <stdio.h>
#include <stdlib.h>

//Data structure for ProcessNode
struct ProcessNode{
    float burst_time;
    float temp_bt;
    float turn_around_time;
    float arrival_time;
    float waiting_time;
    int process_no;
    struct ProcessNode *next;
};

typedef struct ProcessNode PROCESSNODE;
typedef PROCESSNODE *PROCESSNODE_PTR;

//Global linkedlist pointers
PROCESSNODE_PTR master_head = NULL;
PROCESSNODE_PTR ready_queue = NULL;
PROCESSNODE_PTR small_task_head = NULL;
PROCESSNODE_PTR heavy_task_head = NULL;
PROCESSNODE_PTR finished_head = NULL;

//Global CPU time and no of process
int no_process = 0;
float temp_time_taken = 0;

void print_list(PROCESSNODE_PTR list);
void insert_process(int process_no,int burst_time, int arrival_time);
void create_processes();
void sort_by_arrival();
void start_process();
void add_to_ready(PROCESSNODE_PTR node);
void add_to_finish();
void sort_by_burst();
void split_to_small_heavy();
int size_of_list(PROCESSNODE_PTR list);
PROCESSNODE_PTR insert_node_at_end(PROCESSNODE_PTR list,PROCESSNODE_PTR to_add);
void round_robin(PROCESSNODE_PTR queue);
float get_time_quantum(PROCESSNODE_PTR queue);
void set_waiting_time(PROCESSNODE_PTR queue);
void update_waiting_times();