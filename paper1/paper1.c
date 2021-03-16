#include "paper1.h"

// Global linkedlist pointers
PROCESS_PTR process_q_head = NULL;  // Head pointer for process queue
PROCESS_PTR process_q_tail = NULL;  // Tail pointer for process queue

PROCESS_PTR ready_q_head = NULL;    // Head pointer for ready queue

PROCESS_PTR term_q_head = NULL;     // Head pointer for Terminated process queue
PROCESS_PTR term_q_tail = NULL;     // Tail pointer for Terminated process queue

void print_process(PROCESS_PTR process) {
    if (process == NULL) {
        DEBUG("NULL\n");
        return;
    }
    int pid = process->pid;
    DEBUG("***** Process %d [%p] *****\n", pid + 1, process);
    DEBUG("Arrival Time for Process %d: %d\n", pid + 1, process->t_arrival);
    DEBUG("Burst Time for Process %d: %d\n", pid + 1, process->t_exec);
    DEBUG("Remaining Time required: %d\n", process->t_remain);
    DEBUG("Status Flag: %d\n", process->status_flag);
    DEBUG("Turnaround time: %d\n", process->t_turn);
    DEBUG("Waiting time: %d\n", process->t_wait);
    DEBUG("Pointer for next element: %p\n", process->next);
    DEBUG("\n");
}

void print_report(PROCESS_PTR head) {
    // DEVELOPMENT CODE
    // SANITY: PRINT OUT LINKEDLIST FOR VISUALISATION
    PROCESS_PTR p_iter = head;
    DEBUG("********** Linkedlist report **********\n");
    while (p_iter != NULL) {
        print_process(p_iter);
        p_iter = p_iter->next;
    }
    // END DEVELOPMENT CODE
}

/*
 * Insert Process to Process Queue
 *
 * Parameters:
 *   pid       - PID of process in integer form
 *   t_arrival - Arrival time of process
 *   t_exec    - Burst time of Process
 * 
 * Returns:
 *   UNEXPECTED_ARR_TIME  - If arrival time is larger than last arrival time
 *   SUCCESS              - Block have been added to process queue
 */
int insert_process_q(int pid, int t_arrival, int t_exec){
    // Create process and assign known values to process
    PROCESS_PTR p = malloc(sizeof(PROCESS));
    p->pid = pid;
    p->t_arrival = t_arrival;
    p->t_exec = t_exec;
    p->t_remain = t_exec;
    p->status_flag = NEW;
    p->next = NULL;

    if (process_q_head == NULL) {
        // Assign current process to head of process queue if no process is in queue
        process_q_head = p;
        process_q_tail = process_q_head;
    } else {
        // Assign current process to end of process queue
        if (process_q_tail->t_arrival > p->t_arrival){
            // Ensure that current process arrived after last process in queue
            return UNEXPECTED_ARR_TIME;
        }
        process_q_tail->next = p;
        process_q_tail = p;
    }
    return SUCCESS;
}

/*
 * Sort Ready queue using bubble sort
 */
void sort_ready_queue(){
    // Initialise some variables for use in loop
    PROCESS_PTR p_iter = ready_q_head, p_prev = NULL;

    // Bubble sort ready queue
    while (p_iter != NULL && p_iter->next != NULL) {
        if (p_iter->t_remain > p_iter->next->t_remain) {
            // If current process is not in order
            // Remove current process from ready queue
            if (p_prev != NULL) {
                p_prev->next = p_iter->next;
            } else {
                ready_q_head = p_iter->next;
            }

            // Insert removed process to correct position in ready queue
            for (PROCESS_PTR q_iter = ready_q_head->next, prev_q = ready_q_head; q_iter != NULL; prev_q = q_iter, q_iter = q_iter->next) {
                if (prev_q->t_remain < p_iter->t_remain && p_iter->t_remain < q_iter->t_remain) {
                    p_iter->next = prev_q->next;
                    prev_q->next = p_iter;
                    break;
                }
            }
            // Prepare pointers for next iteration
            p_iter = ready_q_head;
            p_prev = NULL;
        } else {
            // Prepare pointers for next iteration
            p_iter = p_iter->next;
        }
    }
}

/*
 * Insert arrival queue into ready queue
 *
 * Parameters:
 *   arrival_q   - Pointer to head of arrival queue
 * 
 */
void insert_ready_q(PROCESS_PTR arrival_q) {
    if (ready_q_head == NULL) {
        // if ready queue is not initialisation
        ready_q_head = arrival_q;
        return;
    }
    
    // Sort ready queue based on remaining time using bubble sort
    sort_ready_queue();
    DEBUG("[After] Sort ready queue\n");
    print_report(ready_q_head);

    // Declare some variables to be used in loop
    // PROCESS_PTR arrival_iter = arrival_q, p_prev = NULL;
    PROCESS_PTR p_prev = NULL;
    PROCESS_PTR ready_iter = ready_q_head; 

    // Add new process to queue using insertion sort
    while (ready_iter != NULL && arrival_q != NULL) {
        DEBUG("ARRIVAL QUEUE\n")
        print_report(arrival_q);
        if (ready_iter->t_remain > arrival_q->t_remain) {
            // If current process in ready queue have a larger remaining time than in current process in arrival queue
            // Remember next process in arrival queue
            PROCESS_PTR temp_process = arrival_q->next;
            if (p_prev == NULL) {
                // If p_prev is null, process is at start of ready_queue
                // Add process to start of ready queue
                arrival_q->next = ready_q_head;
                ready_q_head = arrival_q;
            } else {
                // If p_prev is not null, process is not at the start of ready_queue
                // Add process to queue at correct place
                p_prev->next = arrival_q;
                arrival_q->next = ready_iter;
            }
            // Update pointers to prepare for next loop
            arrival_q = temp_process;
            ready_iter = ready_q_head;
            p_prev = NULL;
            DEBUG("[INSERT READY QUEUE] READY QUEUE\n");
            print_report(ready_q_head);
        } else {
            // Pointer is not at correct position
            // Prepare pointers for next iteration
            p_prev = ready_iter;
            ready_iter = ready_iter->next;
        }
    }

    if (arrival_q != NULL){
        p_prev->next = arrival_q;
    }
}

/*
 * Check if process in process queue should be added to ready queue
 *
 * Parameters:
 *   t_current - Current time
 * 
 * Returns:
 *   PROCESS_PTR  - Pointer to start of arrival queue
 */
PROCESS_PTR check_arrival(int t_current) {
    PROCESS_PTR arrival_q = NULL, prev_head = NULL;
    // Check if there is a process in process queue
    if (process_q_head == NULL) {
        return arrival_q;
    }
    
    if (process_q_head->t_arrival <= t_current) {
        arrival_q = process_q_head;
    } else {
        return arrival_q;
    }

    // Loop through process queue for all process who arrived before current time
    for (PROCESS_PTR p_iter = arrival_q; p_iter != NULL && p_iter->t_arrival <= t_current; p_iter = p_iter->next){
        p_iter->status_flag = READY;
        prev_head = p_iter;
    }

    process_q_head = prev_head->next;
    prev_head->next = NULL;
    return arrival_q;
}

/*
 * Print the process id of processes in the queue
 * Parameters:
 *   q_head     - Pointer to head of queue
 */
void print_queue(PROCESS_PTR q_head) {
    for (PROCESS_PTR process = q_head; process != NULL; process = process->next) {
        printf("%d\t", process->pid);
    }
    printf("\n");
}

/*
 * Main Function
 */
int main() {
    DEBUG("Debug: Enabled\n");
    // Initialise variables needed for process initialisation
    int n_proc = 0, t_quantum = 0, t_current = 0;

    // Request user input for number of processes
    // printf("Enter number of processes: ");
    scanf("%d", &n_proc);

    // Request for time quantum
    // printf("Enter size of the time quantum: ");
    scanf("%d", &t_quantum);

    // Initialise Process Queue
    for (int pid = 0; pid < n_proc; pid++) {
        int t_arrival = 0, t_exec = 0;
        // printf("***** Process %d *****\n", pid + 1);

        // Request arrival time for each process
        // printf("Enter arrival Time for Process %d: ", pid + 1);
        scanf("%d", &t_arrival);

        // Request burst time for each process
        // printf("Enter burst Time for Process %d: ", pid + 1);
        scanf("%d", &t_exec);

        // Add process to Process Queue
        int result = insert_process_q(pid, t_arrival, t_exec);

        // Ensures that process is added successfully
        // Prints error message if error occurs
        if (result == UNEXPECTED_ARR_TIME) {
            printf("Invalid Arrival Time supplied\n");
            pid--;
        }
    }
    printf("Arrangement of processes in ready queue\n");
    // Initialise variables for use for program execution
    int p_term = 0;
    while (process_q_head || ready_q_head) {        
        DEBUG("**************************\n")
        DEBUG("Time: %d\n", t_current);

        // Check if new process arrived
        PROCESS_PTR arrival_q = check_arrival(t_current);
        // Insert to ready queue if new arrival is present
        if (arrival_q) {
            insert_ready_q(arrival_q);
        }
        DEBUG("PROCESS QUEUE\n");
        print_report(process_q_head);
        DEBUG("READY QUEUE\n");
        print_report(ready_q_head);
        DEBUG("CURRENT_PROCESS\n");

        if (!ready_q_head) {
            // if no process is in ready queue
            // Continue to next loop
            continue;
        }

        if (!arrival_q && !p_term) {
            // Round robin if no new process
            PROCESS_PTR p_end = ready_q_head;
            for (p_end; p_end->next != NULL; p_end = p_end->next);
            p_end->next = ready_q_head;
            ready_q_head = ready_q_head->next;
            p_end = p_end->next;
            p_end->next = NULL;
        }


        print_process(ready_q_head);
        print_queue(ready_q_head);
        // "Run" current process
        t_current += t_quantum;
        ready_q_head->t_remain -= t_quantum;

        if (ready_q_head->t_remain <= 0) {
            // Handle if remaining time is negative
            t_current += ready_q_head->t_remain;
            ready_q_head->t_remain = 0;

            // Mark process as terminated
            p_term = 1;
            ready_q_head->status_flag = TERMINATED;
            ready_q_head->t_turn = t_current - ready_q_head->t_arrival;
            ready_q_head->t_wait = ready_q_head->t_turn - ready_q_head->t_exec;

            if (term_q_head) {
                // If terminated queue is initialised
                term_q_tail->next = ready_q_head;
                term_q_tail = ready_q_head;
            } else {
                // If terminated queue is not initialised
                term_q_head = ready_q_head;
                term_q_tail = term_q_head;
            }
            // Remove process from ready queue
            ready_q_head = ready_q_head->next;
            term_q_tail->next = NULL;
            DEBUG("----- After terminate -----\n");
            DEBUG("Iter\n");
            print_process(ready_q_head);
        } else {
            p_term = 0;
        }
        DEBUG("\n\n\n");
    }

    // Initialise variables for use with process analysis
    float total_wait = 0.0, total_turn = 0.0;
    PROCESS_PTR p_iter = term_q_head;
    DEBUG("Terminated queue\n");
    print_report(term_q_head);
    while (p_iter != NULL) {
        // Print process details to screen
        printf("***** Process %d *****\n", p_iter->pid + 1);
        printf("Turnaround Time: %d\n", p_iter->t_turn);
        printf("Waiting Time: %d\n", p_iter->t_wait);

        // Compute total turnaround and waiting time
        total_turn += p_iter->t_turn;
        total_wait += p_iter->t_wait;

        // Prepare pointer for next loop
        p_iter = p_iter->next;
    }

    printf("\n\n");
    printf("Average Waiting Time: %.1f\n", total_wait / n_proc);
    printf("Average Turnaround Time: %.1f\n", total_turn / n_proc);
}