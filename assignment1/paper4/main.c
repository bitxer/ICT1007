// Paper 4 - An Efficent Dynamic Round Robin Algorithm For CPU Scheduling
// Author  - Vernon Tay (P1903047)

#include "paper4.c"

int main() {
    int i, number_of_process, remain_process, timeQuantum;
    int process[max], burst_time[max], remain_time[max], arrival_time[max], arrival_time_copy[max],waiting_Time[max], turnaround_time[max];
    int first_burst_time = 0, highest_burst_time = 0, inital_highest_burst_time = 0, totalExecutionTime = 0, count = 0, number_of_process_with_0_arrival_time = 0, flag =0;
    int *remain_process_ptr, *count_ptr, *number_of_process_with_0_arrival_time_ptr, *flag_ptr;

    /*  Read the number of processes in the system with a maximum of 20 processes allowed and store it to the variable "number_of_process".
        Create a duplicated copy of the total number of processes as the variable "remain_process" as we will need to modify the value later 
        on but we do not want to change the original value of the number of processes.
    */
    printf("Enter the Number of Process(max 20) : ");
    scanf("%d", &number_of_process);
    remain_process = number_of_process;


    /*  Create 2 pointers to point to variable "count" & "number_of_process_with_0_arrival_time" as we can return a single variable "first_burst_time" but we want to 
        make sure that both variable changes in the function using the help of pointers to pass via reference.
        The function "create_processes" will create the number of processes that we want depending on the user input.
    */
    flag_ptr = &flag;
    count_ptr = &count;
    number_of_process_with_0_arrival_time_ptr = &number_of_process_with_0_arrival_time;
    first_burst_time = create_processes(number_of_process, arrival_time, burst_time, remain_time, process, number_of_process_with_0_arrival_time_ptr, first_burst_time, count_ptr,arrival_time_copy,flag_ptr);
    /*  First set the time quantum as 0.8th fraction of the maximum burst time from available processes in ready queue.
        If the arrival time for all processes are 0, first_burst_time = highest_burst_time.
        If the arrival time for all processess are different, it will take the first burst time when arrival time = 0.
    */

    timeQuantum = 0.8 * first_burst_time;

    /*  Create a pointer that points to "remain_process" as we are going to pass into the function "first_process_dynamic_round_robin_execution" but we are only going to return
        the value "total execution time" and the value of "remain_process" will change so we use pointer to pass by reference instead of value. 
    */
    remain_process_ptr = &remain_process;

    /*  Execute the first process using the dynamic round robin algorithm first as we will need to check for possible new processes that arrive after the time slice has ended */
    totalExecutionTime = first_process_dynamic_round_robin_execution(number_of_process, count, number_of_process_with_0_arrival_time, arrival_time, burst_time, timeQuantum, remain_time, totalExecutionTime, waiting_Time, turnaround_time, remain_process_ptr,flag);

    /* Check for the next inital highest burst time in the ready queue and store it in the variable "initial_highest_burst_time"*/
    inital_highest_burst_time = find_initial_highest_burst_time(number_of_process, arrival_time, first_burst_time, burst_time);

    /* Sort processes according to their arrival time. This is not to assume that P1 always have 0 arrival time because P2 can also have 0 arrival time and P1 can have higher arrival time.*/
    sort_by_arrival(number_of_process, arrival_time, process, burst_time, remain_time, waiting_Time, turnaround_time,arrival_time_copy);

    /* Set timeQuantum to 0.8th fraction of the maximum burst time*/
    timeQuantum = 0.8 * inital_highest_burst_time;

    /* Execute the dynamic_round_robin_function for the remaining process 
       The timeQuantum will change dynamically in the function below in accordance to the arrival time of the process and its burst time. 
    */
    flag = 0;

    dynamic_round_robin_function(remain_process, remain_time, timeQuantum, arrival_time, burst_time, highest_burst_time, totalExecutionTime, waiting_Time, turnaround_time, number_of_process,flag_ptr);

    /* Print out the result of the process arrival time, burst time, waiting time, turnaround time, avg waiting time and turnaround time.,*/
    output_result(process, arrival_time_copy, burst_time, waiting_Time, turnaround_time, number_of_process,flag);
    return 0;

}