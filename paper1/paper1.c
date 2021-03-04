#include "paper1.h"

// Global linkedlist pointers
PROCESS_PTR process_q_head = NULL;  // Head pointer for process queue
PROCESS_PTR process_q_tail = NULL;  // Tail pointer for process queue

PROCESS_PTR ready_q_head = NULL;    // Head pointer for ready queue

PROCESS_PTR term_q_head = NULL;     // Head pointer for Terminated process queue
PROCESS_PTR term_q_tail = NULL;     // Tail pointer for Terminated process queue

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

void insert_ready_q(PROCESS_PTR arrival_q, PROCESS_PTR current_process) {
    PROCESS_PTR p_prev = NULL;

    int pid = current_process->pid;
    if (ready_q_head == NULL) {
        // if ready queue is not initialisation
        ready_q_head = arrival_q;
        return;
    }
    for (PROCESS_PTR q_iter = ready_q_head; q_iter->pid != pid; q_iter = q_iter->next) {
        (if )
    }
}

/*
 * Check if process in process queue should be added to ready queue
 *
 * Parameters:
 *   t_current - Current time
 */
PROCESS_PTR check_arrival(int t_current) {
    PROCESS_PTR arrival_q = NULL, prev_head = NULL;
    // Check if there is a process in process queue
    if (process_q_head == NULL) {
        return arrival_q;
    }

    // Loop through process queue for all process who arrived before current time
    for (PROCESS_PTR p_iter; p_iter != NULL; p_iter = p_iter->next){
        p_iter->status_flag = READY;
        if (arrival_q == NULL) {
            // Ready Queue is not initialised
            arrival_q = p_iter;
        }
        prev_head = p_iter;
    }

    if (arrival_q != NULL) {
        process_q_head = prev_head->next;
        prev_head->next = NULL;
    }
    return arrival_q;
}

/*
 * Main Function
 */
int main() {
    // Initialise variables needed for process initialisation
    int n_proc = 0, t_quantum = 0, t_current = 0;

    // Request user input for number of processes
    printf("Enter number of processes: ");
    scanf("%d", &n_proc);

    // Request for time quantum
    printf("Enter size of the time quantum: ");
    scanf("%d", &t_quantum);

    // Initialise Process Queue
    for (int pid = 0; pid < n_proc; pid++) {
        int t_arrival = 0, t_exec = 0;
        printf("***** Process %d *****\n", pid + 1);

        // Request arrival time for each process
        printf("Enter arrival Time for Process %d: ", pid + 1);
        scanf("%d", &t_arrival);

        // Request burst time for each process
        printf("Enter burst Time for Process %d: ", pid + 1);
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

    PROCESS_PTR p_iter = ready_q_head, p_prev = NULL;
    while (process_q_head || ready_q_head) {
        PROCESS_PTR arrival_q = check_arrival(t_current);
        // Insert to ready queue if new arrival is present
        if (arrival_q) {
            insert_ready_q(arrival_q, p_iter);
        }
        
        // Update p_iter pointer
        if (arrival_q || p_iter->next == NULL) {
            p_iter = ready_q_head;
            p_prev = NULL;
        } else {
            p_prev = p_iter;
            p_iter = p_iter->next;
        }

        if (!p_iter) {
            continue;
        }

        // "Run" current process
        t_current += t_quantum;
        p_iter->t_remain -= t_quantum;

        if (p_iter->t_remain <= 0) {
            // Handle if remaining time is negative
            t_current += p_iter->t_remain;
            p_iter->t_remain = 0;
            if (term_q_head) {
                // If terminated queue is initialised
                term_q_tail->next = p_iter;
            } else {
                // If terminated queue is not initialised
                term_q_head = p_iter;
                term_q_tail = term_q_tail;
            }
            // Remove process from process queue
            p_prev->next = p_iter->next;
            p_iter->next = NULL;
        }
    }
}