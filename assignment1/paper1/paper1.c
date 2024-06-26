#include "paper1.h"

// Global linkedlist pointers
PROCESS_PTR process_q_head = NULL;  // Head pointer for process queue
PROCESS_PTR process_q_tail = NULL;  // Tail pointer for process queue

PROCESS_PTR ready_q_head = NULL;    // Head pointer for ready queue

PROCESS_PTR term_q_head = NULL;     // Head pointer for Terminated process queue
PROCESS_PTR term_q_tail = NULL;     // Tail pointer for Terminated process queue

int QUIET = FALSE, INTERACTIVE = FALSE;

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

    // Declare some variables to be used in loop
    // PROCESS_PTR arrival_iter = arrival_q, p_prev = NULL;
    PROCESS_PTR p_prev = NULL;
    PROCESS_PTR ready_iter = ready_q_head; 

    // Add new process to queue using insertion sort
    while (arrival_q != NULL) {
        if (ready_iter == NULL || ready_iter->t_remain > arrival_q->t_remain) {
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
        } else  {
            // Pointer is not at correct position
            // Prepare pointers for next iteration
            p_prev = ready_iter;
            ready_iter = ready_iter->next;
        }
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
    if (QUIET) {
        return;
    }
    for (PROCESS_PTR process = q_head; process != NULL; process = process->next) {
        VERBOSE("%d\t", process->pid);
    }
    VERBOSE("\n");
}

/*
 * Print help message for program
 * Parameters:
 *   exe_name   - Name used to run current instance of executable
 */
void print_help(char * exe_name) {
    printf("Usage: %s [-iq] [file...]\n", exe_name);
    printf("  -i      Run program in interactive mode\n");
    printf("  -q      Prompts for input will be silenced and only final statistics\n");
    printf("          will be printed\n");
}

/*
 * Print control variables set via command line
 */
void print_config() {
    printf("==============================================================================================\n");
    switch (QUIET){
    case TRUE:
        printf("Quiet Mode: Enabled\n");
        break;
    default:
        printf("Quiet Mode: Disabled\n");
    }

    switch (INTERACTIVE){
    case TRUE:
        printf("Interactive Mode: Enabled\n");
        break;
    default:
        printf("Interactive Mode: Disabled\n");
    }
    printf("==============================================================================================\n");
}

/*
 * Main Function
 */
int main(int argc, char * argv[]) {
    char opt;
    while ((opt = getopt(argc, argv, "iq")) != -1) {
        switch (opt) {
        case 'i': 
            INTERACTIVE = TRUE;
            break;
        case 'q':
            QUIET = TRUE;
            break;
        default:
            print_help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    print_config();

    // Initialise variables needed for process initialisation
    int n_proc = 0, t_quantum = 0, t_current = 0;

    // Request user input for number of processes
    PROMPT("Enter number of processes: ");
    scanf("%d", &n_proc);

    // Request for time quantum
    PROMPT("Enter size of the time quantum: ");
    scanf("%d", &t_quantum);

    // Initialise Process Queue
    for (int pid = 1; pid <= n_proc; pid++) {
        int t_arrival = 0, t_exec = 0;
        PROMPT("***** Process %d *****\n", pid);

        // Request arrival time for each process
        PROMPT("Enter arrival Time for Process %d: ", pid);
        scanf("%d", &t_arrival);

        // Request burst time for each process
        PROMPT("Enter burst Time for Process %d: ", pid);
        scanf("%d", &t_exec);

        // Add process to Process Queue
        int result = insert_process_q(pid, t_arrival, t_exec);

        // Ensures that process is added successfully
        // Prints error message if error occurs
        if (result == UNEXPECTED_ARR_TIME) {
            printf("Invalid Arrival Time supplied for process %d\n", pid);
            pid--;
        }
    }
    PROMPT("==============================================================================================\n");
    printf("Number of Processes: %d\n", n_proc);
    printf("Time Quantum: %d\n", t_quantum);
    printf("==============================================================================================\n");
    VERBOSE("Arrangement of processes in ready queue\n");
    // Initialise variables for use for program execution
    int p_term = 0;
    while (process_q_head || ready_q_head) {

        // Check if new process arrived
        if (ready_q_head == NULL) {
            t_current = process_q_head->t_arrival;
        }
        PROCESS_PTR arrival_q = check_arrival(t_current);
        // Insert to ready queue if new arrival is present
        if (arrival_q) {
            insert_ready_q(arrival_q);
        }

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
        } else {
            p_term = 0;
        }
    }

    VERBOSE("==============================================================================================\n");
    printf("Process ID\tArrival Time\tBurst Time\tTurnaround Time\t\tWaiting Time\n");
    // Initialise variables for use with process analysis
    float total_wait = 0.0, total_turn = 0.0;
    PROCESS_PTR p_iter = term_q_head;
    while (p_iter != NULL) {
        // Print process details to screen
        printf("%6d\t%15d\t%13d\t%15d\t\t%15d\n", p_iter->pid, p_iter->t_arrival, p_iter->t_exec, p_iter->t_turn, p_iter->t_wait);

        // Compute total turnaround and waiting time
        total_turn += p_iter->t_turn;
        total_wait += p_iter->t_wait;

        // Prepare pointer for next loop
        p_iter = p_iter->next;
    }
    printf("==============================================================================================\n");
    printf("Average Waiting Time: %.2f\n", total_wait / n_proc);
    printf("Average Turnaround Time: %.2f\n", total_turn / n_proc);
    printf("==============================================================================================\n");
}