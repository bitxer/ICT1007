// Paper 4 - An Efficent Dynamic Round Robin Algorithm For CPU Scheduling
// Author  - Vernon Tay (P1903047)

#include "paper4.h"


/*    This function reads the Arrival Time and CPU Burst time for each processes. It also creates a duplicated copy of the CPU burst time for each processes as the variable "remain_time"
      as we also need to modify this value later on but we do not want to change the original burst time
      process[i] stores the process number itself. E.g process[0] = 1, process[1] = 2 which we will make use of this to display out the process number
      Check for arrival time = 0 where it determines the start of the process and then find the maximum burst time from available processes in the ready queue.
      If all of the processes have 0 arrival time, it will find the maximum burst time.
      If 2 processes have 0 arrival time, it will find the maximum burst time within the 2 processes.
      If all processes have different arrival time, it will find the inital burst time when arrival time is 0.
      Store the total number of "processes with 0 arrival time" & store the iterator with the maximum burst time when arrival time is 0 to the pointer.
      Pointers were used because we can only return 1 variable in this function but we want to make sure the other variables changes their value in memory
*/

int create_processes(int number_of_process, int arrival_time[], int burst_time[], int remain_time[], int process[], int *number_of_process_with_0_arrival_time_ptr, int first_burst_time, int *count_ptr) {
    for (int i = 0; i < number_of_process; i++) {
        printf("Enter Details of Process[%d]\n", i + 1);
        printf("Enter Arrival Time: ");
        scanf("%d", &arrival_time[i]);
        printf("Enter Burst Time: ");
        scanf("%d", &burst_time[i]);
        remain_time[i] = burst_time[i];
        process[i] = i + 1;
        if (arrival_time[i] == 0) {
            (*number_of_process_with_0_arrival_time_ptr)++;
            if (first_burst_time < burst_time[i]) {
                first_burst_time = burst_time[i];
                *count_ptr = i;
            }
        }

    }
    return first_burst_time;
}




/*    It will skip the process with the highest burst time as we want to keep the larger process at the back.
      The scheduler then assigns the CPU to the first process with arrival time 0. If there are 2 processes with arrival time 0, it
      will skip the process with the highest burst time and execute on another process. If there are only 1 process with arrival time 0,
      it will assign the CPU to that process instead.
      If the burst time is more than the time quantum, the burst time of the process will be subtracted by the time quantum and the total execution will increment by the time quantum
      If the burst time is less than  or equal to the time quantum, totalexecutiontime will be added to the burst time, waiting time and turnaround time can then be caculated.
      We then set that current burst time to zero as it has finished executing and the number of remain_process will decrease by 1.
      For both condition, it will break after the first process is assigned to the CPU as we will then need to check for possible new processes that arrive after this time slice has ended.
*/


int first_process_dynamic_round_robin_execution(int number_of_process, int count, int number_of_process_with_0_arrival_time, int arrival_time[], int burst_time[], int timeQuantum, int remain_time[], int totalExecutionTime, int waiting_Time[], int turnaround_time[], int *remain_process_ptr) {

    for (int i = 0; i < number_of_process; i++) {
        if (count == i && number_of_process_with_0_arrival_time > 1) {
            continue;
        } else if (arrival_time[i] == 0  && burst_time[i] > timeQuantum) {
            remain_time[i] = remain_time[i] - timeQuantum;
            totalExecutionTime += timeQuantum;
            break;
        } else if (arrival_time[i] == 0 && burst_time[i] <= timeQuantum) {
            totalExecutionTime += remain_time[i];
            waiting_Time[i] = totalExecutionTime - arrival_time[i] - burst_time[i];
            turnaround_time[i] = totalExecutionTime - arrival_time[i];
            remain_time[i] = 0;
            (*remain_process_ptr)--;
            break;
        }

    }

    return totalExecutionTime;
}


/*    Check for the inital highest burst time in the ready queue by comparing the arrival time of each process with the inital highest time quantum when the arrival time is 0.
      This is set such that if a process has the highest burst time but it haven't arrive, its burst time will not be taken into account.
      Thus, this function checks for any new processes that are entering the ready queue and it will recaculate the highest burst time again.
*/


int find_initial_highest_burst_time(int number_of_process, int arrival_time[], int first_burst_time, int burst_time[]) {
    int first_highest_burst_time = 0;
    for (int i = 0; i < number_of_process; i++) {
        if (arrival_time[i] < (0.8 * first_burst_time)) {
            if (burst_time[i] > first_highest_burst_time) {
                first_highest_burst_time = burst_time[i];
            }

        }
    }
    return first_highest_burst_time;

}



/*    Swapping of processes via selection sort where it will sort processes via shortest arrival time . This solves the scenario in the test data given in the assignment where they always set the
      first process to have the shortest arrival time and it increments via the process number. However, with this implementation, it adds flexibility where you can set the second process
      to have shorter arrival time than the first process and still gets the same results.
*/

void sort_by_arrival(int number_of_process, int arrival_time[], int process[], int burst_time[], int remain_time[], int waiting_Time[] , int turnaround_time[]) {
    int pos, j, temp;
    for (int i = 0; i < number_of_process; i++) {
        pos = i;
        for (j = i + 1; j < number_of_process; j++) {
            if (arrival_time[j] < arrival_time[pos]) { //loop through all the process to see which one is the shortest
                pos = j;
            }
        }
        /* Swapping the value of the arrival time of the process. */
        temp  = arrival_time[i];
        arrival_time[i] = arrival_time[pos];
        arrival_time[pos] = temp;

        /* Swapping the process number */
        temp = process[i];
        process[i] = process[pos];
        process[pos] = temp;


        /* Swapping the value of the burst time of the process. */
        temp = burst_time[i];
        burst_time[i] = burst_time[pos];
        burst_time[pos] = temp;


        /* Swapping the value of the remaining time of the process. */
        temp = remain_time[i];
        remain_time[i] = remain_time[pos];
        remain_time[pos] = temp;

        /* Swapping the value of the waiting time of the process. */
        temp = waiting_Time[i];
        waiting_Time[i] = waiting_Time[pos];
        waiting_Time[pos] = temp;

        /* Swapping the value of the turnaround time of the process. */
        temp = turnaround_time[i];
        turnaround_time[i] = turnaround_time[pos];
        turnaround_time[pos] = temp;


    }

}


/*      This function assigns the CPU to all the processes in ready queue with burst time less than the time quantum while larger ones are kept hold on.
        If burst time is less than time quantum and more than 0, it will assign CPU to the process. the total execution time will be added to the burst time,
        the burst time of the process will be set to 0 and the number of remaining process will decrement by one which shows that the process has finished execution.
        The scheduler recalculates the time quantum at the end of current quantum by comparing the arrival time of each processes with the totalExecutionTime.
        It then finds the highest burst time within the ready queue and recomputes its timeQuantum.
        If burst time is higher than the time quantum and more than 0, it will put the process at the end of ready queue
        As soon as all the smaller processes complete their execution, the time quantum is set equal to maximum burst time.

*/

void dynamic_round_robin_function(int remain_process, int remain_time[], int timeQuantum, int arrival_time[], int burst_time[], int highest_burst_time, int totalExecutionTime, int waiting_Time[], int turnaround_time[], int number_of_process) {
    for (int i = 0; remain_process != 0;) {
        for (int k = 0; k < number_of_process; k++) {
            if (arrival_time[k] < totalExecutionTime) {
                if (burst_time[k] > highest_burst_time) {
                    highest_burst_time = burst_time[k];
                    timeQuantum = highest_burst_time * 0.8;
                }

            }
        }
        if (remain_time[i] <= timeQuantum && remain_time[i] > 0) {
            totalExecutionTime += remain_time[i];
            waiting_Time[i] = totalExecutionTime - arrival_time[i] - burst_time[i];
            turnaround_time[i] = totalExecutionTime - arrival_time[i];
            remain_time[i] = 0;
            remain_process--;
        }


        else if (remain_time[i] > timeQuantum) {
            waiting_Time[i] = 0;
            turnaround_time[i] = 0;

        }




        /*  This resets the loop once all small processes have finished executing and then the the time quantum is set equal to maximum burst time. */
        if (i == number_of_process - 1) {
            timeQuantum = highest_burst_time;
            i = 0;
        }
        /*  This checks whether the next process is in the queue after executing the current one, if it is in the queue, it will increment by one to move to the next process */
        else if (totalExecutionTime >= arrival_time[i + 1]) {
            i++;
        }
        /* This condition ensures that the last process could arrive during the execution of the second last process. In example 2, P6 can arrive after time = 208 onwards whike
           P5 is still executing */
        else if ((totalExecutionTime >= arrival_time[i + 1]) || remain_process > 1) {
            timeQuantum = highest_burst_time;
            i = 0;
        } else {
            i = 0;
        }
    }

}




/*  This function calculate and print the waiting time, turn aroundtime for each process and the avg waiting and turnaround time for all processes*/

void output_result(int process[], int arrival_time[], int burst_time[], int waiting_Time[], int turnaround_time[], int number_of_process) {
    float avg_waiting_time = 0.0, avg_turnaround_time = 0.0;
    printf("Process\t Arrival Time\t Burst Time\t Waiting Time\t Turnaround_time\n");
    for (int i = 0; i < number_of_process; i++) {
        avg_waiting_time    += waiting_Time[i];
        avg_turnaround_time += turnaround_time[i];
        printf("P[%d]\t\t%d\t\t%d\t\t%d\t\t%d\n", process[i], arrival_time[i], burst_time[i], waiting_Time[i], turnaround_time[i]);
    }

    avg_waiting_time = avg_waiting_time / number_of_process;
    avg_turnaround_time = avg_turnaround_time / number_of_process;
    printf("\n\nAverage Waiting Time:\t%.1f", avg_waiting_time);
    printf("\nAvg Turnaround Time:\t%.1f", avg_turnaround_time);

}





