#include <stdio.h>
#include <stdlib.h>

int check_for_possible_finish(int burst, int quantum, int priority)
{
    //Following the formula in the white-paper, checks if the remaining burst time is sufficiently
    //low enough relative to its priority that it can complete the entire process burst rather than wait
    if (burst <= quantum)
    {
        return 1;
    }
    else if (priority == 3)
    {
        if (burst <= quantum + (0.30 * quantum))
        {
            return 1;
        }
    }
    else
    {
        if (burst<= quantum + (0.20 * quantum))
        {
            return 1;
        }
        return 0;
    }
    //Shouldn't reach here
    return 0;
}

int main() {
    //Requested --> quantum_med, number_of_processes
    //Per-Process   --> burst_time, arrival_time, priority
    int number_of_processes,quantum_high, quantum_med, quantum_low,

    //Burst Time, Arrival Time, Priority and Flag per process
    burst_time[20], arrival_time[20], priority[20], flag[20],

    //Backup Data
    original_burst_time[20],

    //Calculated values
    waiting_time[20], turnaround_time[20], average_waiting_time = 0, average_turnaround_time = 0,

    //Quantum for each process
    quantum_array[20];

    //System Counter is the amount of time since the start
    int system_counter = 0,
    //Number of context switches
    context_switches = 0,
    //All processes completed flag
    completed = 0,
    //Idle Time
    idle_time = 0;

    //Linked-List Structure
    struct Node {
        int data;
        int index;
        struct Node* next;
    };

    struct Node* head = NULL;
    struct Node* temp = NULL;
    struct Node* tail = NULL;

    head = (struct Node*)malloc(sizeof(struct Node));
    temp = (struct Node*)malloc(sizeof(struct Node));
    tail = (struct Node*)malloc(sizeof(struct Node));

    head->data = -1;

    //User Input
    printf("Number of processes:");
    scanf("%d", &number_of_processes);

    printf("Quantum:");
    scanf("%d", &quantum_med);

    for (int i=0; i < number_of_processes; i++)
    {
        printf("Process %d burst time:", i);
        scanf("%d", &burst_time[i]);
        original_burst_time[i] = burst_time[i];

        printf("Process %d priority:", i);
        scanf("%d", &priority[i]);

        printf("Process %d arrival time:", i);
        scanf("%d", &arrival_time[i]);
    }

    int threshold = quantum_med * 0.20; //Arbitrary threshold

    //Identify any low-BT processes and finish them first
    //Linked-List of processes below threshold
    for (int i=0; i < number_of_processes; i++)
    {
        if (burst_time[i] <= threshold)
        {
            //New Node
            struct Node* newNode = NULL;
            newNode = (struct Node*)malloc(sizeof(struct Node));
            newNode -> data = burst_time[i];
            newNode -> index = i;

            //Beginning of linked list is empty, lets initialise it
            if (head->data == -1)
            {
                head = newNode;
            }
            //Check if head is already bigger
            else if (head->data > newNode->data)
            {
                newNode->next = head;
                head = newNode;
            }
            else
            {
                temp = head;
                while (temp->next != NULL)
                {
                    if(temp->data < burst_time[i] && temp->next->data > burst_time[i])
                    {
                        newNode->next = temp->next;
                        temp->next = newNode;
                        break;
                    }
                    temp = temp->next;
                }
                if (temp->next==NULL && temp->data < burst_time[i])
                {
                    temp->next = newNode;
                }
            }
        }
    }

    //Set all flags to False by default
    for (int i=0; i < 20; i++)
    {
        flag[i] = 0; //FALSE
    }

    //Finish the processes
    temp = head;
    while (temp != NULL)
    {
        if (temp->data == -1)
        {
            break;
        }
        int current_index = temp->index;
        system_counter += temp->data;
        waiting_time[current_index] = system_counter - temp->data;
        turnaround_time[current_index] = system_counter - arrival_time[current_index];
        flag[current_index] = 1;
        printf("%d (%d)| ", current_index, system_counter);
        temp = temp->next;
        context_switches += 1;
    }


    //Calculate the appropriate Time Quantum

    quantum_low = quantum_med - (0.20 * quantum_med);
    quantum_high = quantum_med + (0.20 * quantum_med);

    quantum_array[1] = quantum_low;
    quantum_array[2] = quantum_med;
    quantum_array[3] = quantum_high;

    //Begin improved Round-Robin
    int current_system_counter = -1;
    //Check for if we even did any work this time by comparing before-and-after processing
    //If processes are processed, system counter will have changed but not current system counter
    //while (current_system_counter != system_counter)
    while (completed == 0)
    {
        current_system_counter = system_counter;
        for (int i = 0; i < number_of_processes; i++)
        {
            //Check if process is already completed
            if (flag[i] == 1)
            {
                continue;
            }
            //Check if process has arrived yet
            if (arrival_time[i] > system_counter)
            {
                continue;
            }

            //Standard procedure
            //Check if burst time of process is less than or equal to quantum
            if (burst_time[i] <= quantum_array[priority[i]] )
            {
                system_counter += burst_time[i];
                burst_time[i] = 0;
                waiting_time[i] = system_counter - original_burst_time[i] - arrival_time[i];
                turnaround_time[i] = system_counter - arrival_time[i];
                flag[i] = 1;
            }
            //Context-saving Mechanic
            //Check if eligible for fast-finish
            else if (check_for_possible_finish(burst_time[i], quantum_array[priority[i]], priority[i]) == 1)
            {
                system_counter += burst_time[i];
                burst_time[i] = 0;
                waiting_time[i] = system_counter - original_burst_time[i] - arrival_time[i];
                turnaround_time[i] = system_counter - arrival_time[i];
                flag[i] = 1;
            }
            //Do it the old-fashioned way
            else
            {
                system_counter += quantum_array[priority[i]];
                burst_time[i] -= quantum_array[priority[i]];
            }
            printf("%d (%d)| ", i, system_counter);
            //We're done with this process, we switch NOW (unless its the same process of course) *Potentially Inaccurate*
            context_switches += 1;
        }
        if (current_system_counter == system_counter)
        {
            completed = 1;
            for (int i = 0; i < number_of_processes; i++)
            {
                //Check if there are processes that are still not completed yet BUT not arrived yet
                if (flag[i] == 0)
                {
                    //Signify that the scheduler isnt done yet
                    completed = 0;
                    //Increase system counter by 10 to signify idle time
                    system_counter += 10;
                    idle_time += 10;
                    break;
                }
            }
        }
    }

    //Account for one extra context switch processing the end of the process
    //There is NO context switch going from last process to idle
    context_switches -= 1;

    //All processes ended
    //Calculate total waiting time and turnaround time
    for (int i=0; i < number_of_processes; i++)
    {
        average_waiting_time += waiting_time[i];
        average_turnaround_time += turnaround_time[i];
    }
    //Calculate average waiting time and average turnaround time
    average_waiting_time = average_waiting_time / number_of_processes;
    average_turnaround_time = average_turnaround_time / number_of_processes;

    //Print out the burst time, waiting time and turn around time of each process
    printf("\n");
    printf("\n%-20s %-20s %-20s", "Burst Time", "Waiting Time", "Turn Around Time");
    for (int i=0; i < number_of_processes; i++)
    {
        printf("\n%-20d %-20d %-20d", original_burst_time[i], waiting_time[i], turnaround_time[i]);
    }

    //Print AWT,ATT
    printf("\n\n");
    printf("Average Waiting Time: %d\n", average_waiting_time);
    printf("Average Turnaround Time: %d\n", average_turnaround_time);
    printf("Number of context switches: %d\n", context_switches);
    printf("Idle CPU Time: %d", idle_time);


    return 0;
}
