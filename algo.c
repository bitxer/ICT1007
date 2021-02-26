#include "algo.h"
#include "getopt.h"

void sortedInsert(ProcessList** head, ProcessList* node, int sortBy) {
    if (*head == NULL) {
        *head = node;
        return;
    }

    ProcessList* current = *head;
    int node_field = 0;
    int current_field = 0;
    int next_field = 0;

    if (sortBy == BURST_TIME) {
        node_field = node->burst_time;
        current_field = current->burst_time;
        if (current->next != NULL) {
            next_field = current->next->burst_time;
        }
    } else if (sortBy == ARRIVAL_TIME) {
        node_field = node->arrival_time;
        current_field = current->arrival_time;
        if (current->next != NULL) {
            next_field = current->next->arrival_time;
        }

    } else {
        printf("help la what you doing\n");
        return;
    }

    if (node_field <= current_field) {
        node->next = current;
        *head = node;
        return;
    }

    while (current != NULL) {
        if (node_field >= current_field && (current->next == NULL || node_field < next_field)) {
            node->next = current->next;
            current->next = node;
            return;
        }
        
        current = current->next;
    }
}

ProcessList* createProcess(int process_id, int burst_time, int arrival_time, int waiting_time, int turnaround_time) {
    ProcessList* newProc = malloc(sizeof(ProcessList));
    newProc->process_id = process_id;
    newProc->burst_time = burst_time;
    newProc->arrival_time = arrival_time;
    newProc->waiting_time = 0;
    newProc->turnaround_time = 0;
    newProc->next = NULL;

    return newProc;
}

ProcessList* copyProcess(ProcessList* source) {
    int process_id = source->process_id;
    int arrival_time = source->arrival_time;
    int burst_time = source->burst_time;
    int waiting_time = source->burst_time;
    int turnaround_time = source->burst_time;

    return createProcess(process_id, burst_time, arrival_time, waiting_time, turnaround_time);
}

void printProcessList(ProcessList*  n) {
    while (n != NULL) {
        if (n->next == NULL) {
            printf("P%d\n", n->process_id);
        } else {
            printf("P%d -> ", n->process_id);
        }
        n = n->next;
    }
}

void runProcesses(ProcessList* ready_queue, ProcessList* incoming_arrivals, int* time, int* k_factor, int* total_num_of_processes) {
    int num_of_processes = 0;

    printf("time: %d\n", *time);

    for (*time; num_of_processes < *total_num_of_processes; *time = *time+1) {

        // Check incoming arrivals if there are expected processes
        // Insert the process into the ready queue
        // Pop the inserted process and check if next process has the same time
        // Otherwise break
        for (;;) {
            if (incoming_arrivals->arrival_time <= *time) {
                // create a copy of the new process and insert into the ready queue
                // have to create a copy if not the linked list will follow
                sortedInsert(&ready_queue, copyProcess(incoming_arrivals), BURST_TIME);
                incoming_arrivals = incoming_arrivals->next;
            } else {
                break;
            }
        }
        
        
        if (ready_queue == NULL) {
            printf("Time passed:%d\n", *time);
            printf("No process to run\n");
            continue;
        }

        ProcessList* previous = NULL;

        while (ready_queue != NULL) {
            printf("======================\n");
            printf("Time passed: %d\n", *time);
            if (num_of_processes == 0) {
                printf("Running PID %d for %d cycles\n", ready_queue->process_id, ready_queue->burst_time);

                *time += ready_queue->burst_time;
                ready_queue->turnaround_time = *time - ready_queue->arrival_time;
                ready_queue->waiting_time = ready_queue->turnaround_time - ready_queue->burst_time;
                printf("Turnaround time for PID %d: %d\n", ready_queue->process_id, ready_queue->turnaround_time);
                printf("Waiting time for PID %d: %d\n", ready_queue->process_id, ready_queue->waiting_time);
                previous = ready_queue;
                ready_queue = ready_queue->next;
                num_of_processes++;
                continue;
            }

            // increment K factor alternatively
            if (K_INCREMENT == 1) {
                if (num_of_processes % 2 == 0) {
                    *k_factor = *k_factor + 1;
                }
            }
            

            if (ready_queue->next != NULL) {
                ProcessList* ProcessReadyOne = ready_queue;
                ProcessList* ProcessReadyTwo = ready_queue->next;

                int first = *k_factor * ProcessReadyOne->burst_time;
                int second = *time + ProcessReadyTwo->burst_time;

                if (DEBUG_MODE) {
                    printf("first ready process burst time: %d\n", ProcessReadyOne->burst_time);
                    printf("second ready process burst time: %d\n", ProcessReadyTwo->burst_time);
                    printf("k factor: %d\n", *k_factor);
                    printf("(k * first): %d\n", first);
                    printf("(t + second): %d\n", second);
                    printf("first > second: %d\n", first > second);
                    if (first > second) {
                        printf("CPU goes to second process P%d\n", ProcessReadyTwo->process_id);
                    } else {
                        printf("CPU goes to first process P%d\n", ProcessReadyOne->process_id);
                    }
                }

                if (first > second) {
                    if (previous != NULL) {
                        previous->next = ProcessReadyTwo;
                    }
                    ProcessReadyOne->next = ProcessReadyTwo->next;
                    ProcessReadyTwo->next = ProcessReadyOne;
                    ready_queue = ProcessReadyTwo;
                }
            }
            
            printf("Running PID %d for %d cycles\n", ready_queue->process_id, ready_queue->burst_time);
            *time += ready_queue->burst_time;
            ready_queue->turnaround_time = *time - ready_queue->arrival_time;
            ready_queue->waiting_time = ready_queue->turnaround_time - ready_queue->burst_time;
            printf("Turnaround time for PID %d: %d\n", ready_queue->process_id, ready_queue->turnaround_time);
            printf("Waiting time for PID %d: %d\n", ready_queue->process_id, ready_queue->waiting_time);
            previous = ready_queue;
            ready_queue = ready_queue->next;
            num_of_processes++;
            
            printf("======================\n");
        }
    }

    printf("Number of processes ran: %d\n", num_of_processes);
    
}

int getTestData(char* filepath, int* num_of_processes, ProcessList** process_list, ProcessList** ready_queue, ProcessList** incoming_arrivals) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    
    int process_id = 0;
    int arrival_time = 0;
    int burst_time = 0;

    if (filepath == NULL) {
        filepath = "./data.csv";
    }

    fp = fopen(filepath, "r");
    if (fp == NULL)
        return -1;

    while ((read = getline(&line, &len, fp)) != -1) {
        sscanf(line, "%d, %d, %d\n", &process_id, &burst_time, &arrival_time);

        ProcessList* newProc = NULL;
        newProc = createProcess(process_id, burst_time, arrival_time, 0, 0);

        
        sortedInsert(process_list, newProc, ARRIVAL_TIME);

        if (arrival_time == 0) {
            // Ready Queue should be sorted by burst time (SJF)
            sortedInsert(ready_queue, newProc, BURST_TIME);
        } else {
            // 
            sortedInsert(incoming_arrivals, newProc, ARRIVAL_TIME);
        }
        *num_of_processes = *num_of_processes + 1;
    }

    fclose(fp);
    if (line)
        free(line);
    
    return 0;
}

void printHelp(const char* name, const char* option) {
    printf("unknown option: %s\n", option);
    printf("usage:\t%s [flags]\n", name);
    printf("-d\tdebug mode\n");
    printf("-h\tprint this help menu\n");
    printf("-i\tspecify input filepath\n");
    printf("-k\tincreases k factor on the alternate loops\n");
    return;
}

int main(int argc, char* argv[]) {
    int k_factor = 2;
    int time = 0;
    int opt = 0;
    int total_num_of_processes = 0;
    char* filepath = NULL;

    if (argc > 1) {
        while ((opt = getopt(argc, argv, "dhki:")) != -1) {
            switch (opt) {
                case 'd':
                    DEBUG_MODE = 1;
                    break;
                case 'h':
                    printHelp(argv[0], argv[1]);
                    return 0;
                case 'k':
                    K_INCREMENT = 1;
                    break;
                case 'i':
                    filepath = optarg;
                    break;
                default: /* '?' */
                    printHelp(argv[0], argv[1]);
                    return -1;
            }
        }
    }

    int resp = 0;
    // Process List:
    // Track all processes regardless of arrival time
    ProcessList* process_list = NULL;

    // Ready Queue: 
    // Processes that arrive at t=0; Should be sorted by descending burst time
    ProcessList* ready_queue = NULL;

    // Incoming Arrivals:
    // Processes that arrive after t=0; Should be sorted by ascending arrival time
    ProcessList* incoming_arrivals = NULL;

    resp = getTestData(filepath, &total_num_of_processes, &process_list, &ready_queue, &incoming_arrivals);

    if (resp != 0) {
        printf("unable to open file: %s\n", filepath);
        return -1;
    }

    printf("number of data: %d\n", total_num_of_processes);


    printf("SJF Process List: ");

    printProcessList(process_list);
    runProcesses(ready_queue, incoming_arrivals, &time, &k_factor, &total_num_of_processes);
    
    // printf("Resultant process order: ");
    // printProcessList(process_list);

}
