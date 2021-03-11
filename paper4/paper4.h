// Paper 4 - An Efficent Dynamic Round Robin Algorithm For CPU Scheduling
// Author  - Vernon Tay (P1903047)

#include<stdio.h>
#define max 20  /* Create a maximum size for the arrays later.*/


int create_processes(int number_of_process, int arrival_time[], int burst_time[], int remain_time[], int process[], int *number_of_process_with_0_arrival_time_ptr, int first_burst_time, int *count_ptr);
int first_process_dynamic_round_robin_execution(int number_of_process, int count, int number_of_process_with_0_arrival_time, int arrival_time[], int burst_time[], int timeQuantum, int remain_time[], int totalExecutionTime, int waiting_Time[], int turnaround_time[], int *remain_process_ptr);
int find_initial_highest_burst_time(int number_of_process, int arrival_time[], int first_burst_time, int burst_time[]);
void sort_by_arrival(int number_of_process, int arrival_time[], int process[], int burst_time[], int remain_time[], int waiting_Time[], int turnaround_time[]);
void dynamic_round_robin_function(int remain_process, int remain_time[], int timeQuantum, int arrival_time[], int burst_time[], int highest_burst_time, int totalExecutionTime, int waiting_Time[], int turnaround_time[], int number_of_process);
void output_result(int process[], int arrival_time[], int burst_time[], int waiting_Time[], int turnaround_time[], int number_of_process);
