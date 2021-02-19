#include "algo.h"
#include "getopt.h"

void sortedInsert(ProcessList** head, ProcessList* node) {
    if (*head == NULL) {
        *head = node;
        return;
    }

    ProcessList* current = *head;

    if (node->burst_time < current->burst_time) {
        node->next = current;
        *head = node;
        return;
    }

    while (current != NULL) {
        if (node->burst_time >= current->burst_time && (current->next == NULL || node->burst_time < current->next->burst_time)) {
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
    newProc->arrival_time = arrival_time;
    newProc->burst_time = burst_time;
    newProc->turnaround_time = 0;
    newProc->waiting_time = 0;
    newProc->next = NULL;

    return newProc;
}

void printProcessList(ProcessList*  n) {
    while (n != NULL) {
        if (n->next->next == NULL) {
            printf("P%d -> P%d\n", n->process_id, n->next->process_id);
            return;
        } else {
            printf("P%d -> ", n->process_id);
        }

        n = n->next;
    }
}

void runProcesses(ProcessList* processes, int* time, int* k_factor) {
    int num_of_processes = 0;

    if (processes == NULL) {
        printf("No processes in waiting queue.\n");
        return;
    }

    ProcessList* previous = NULL;

    while (processes != NULL) {
        printf("======================\n");
        printf("Time passed: %d\n", *time);
        if (num_of_processes == 0) {
            printf("Running PID %d for %d cycles\n", processes->process_id, processes->burst_time);

            *time += processes->burst_time;
            processes->turnaround_time = *time - processes->arrival_time;
            processes->waiting_time = processes->turnaround_time - processes->burst_time;
            printf("Turnaround time for PID %d: %d\n", processes->process_id, processes->turnaround_time);
            printf("Waiting time for PID %d: %d\n", processes->process_id, processes->waiting_time);
            previous = processes;
            processes = processes->next;
            num_of_processes++;
            continue;
        }

        // increment K factor alternatively
        if (K_INCREMENT == 1) {
            if (num_of_processes % 2 == 0) {
                *k_factor = *k_factor + 1;
            }
        }
        

        if (processes->next != NULL) {
            ProcessList* ProcessReadyOne = processes;
            ProcessList* ProcessReadyTwo = processes->next;

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
                processes = ProcessReadyTwo;
            }
        }
        
        printf("Running PID %d for %d cycles\n", processes->process_id, processes->burst_time);
        *time += processes->burst_time;
        processes->turnaround_time = *time - processes->arrival_time;
        processes->waiting_time = processes->turnaround_time - processes->burst_time;
        printf("Turnaround time for PID %d: %d\n", processes->process_id, processes->turnaround_time);
        printf("Waiting time for PID %d: %d\n", processes->process_id, processes->waiting_time);
        previous = processes;
        processes = processes->next;
        num_of_processes++;
        
        printf("======================\n");
    }

    printf("Number of processes ran: %d\n", num_of_processes);
    
}

ProcessList* getTestData(char* filepath) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    
    int process_id = 0;
    int arrival_time = 0;
    int burst_time = 0;

    ProcessList* head = NULL;

    if (filepath == NULL) {
        filepath = "./data.csv";
    }

    fp = fopen(filepath, "r");
    if (fp == NULL)
        return NULL;

    while ((read = getline(&line, &len, fp)) != -1) {
        sscanf(line, "%d, %d, %d\n", &process_id, &burst_time, &arrival_time);

        ProcessList* newProc = NULL;
        newProc = createProcess(process_id, burst_time, arrival_time, 0, 0);
        sortedInsert(&head, newProc);
    }

    fclose(fp);
    if (line)
        free(line);
    
    return head;
}

void printHelp(const char* name, const char* option) {
    printf("unknown option: %s\n", option);
    printf("usage:\t%s [flags]\n", name);
    printf("-d\tdebug mode\n");
    printf("-h\tprint this help menu\n");
    printf("-i\tspecify input filepath");
    printf("-k\tincreases k factor on the alternate loops\n");
    return;
}

int main(int argc, char* argv[]) {
    int k_factor = 2;
    int time = 0;
    int opt = 0;
    char* filepath = NULL;

    if (argc > 1) {
        while ((opt = getopt(argc, argv, "dhki:")) != -1) {
            switch (opt) {
                case 'd':
                    DEBUG_MODE = 1;
                    break;
                case 'h':
                    printHelp(argv[0], argv[1]);
                    break;
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

    ProcessList* process_list = NULL;


    process_list = getTestData(filepath);

    if (process_list == NULL) {
        printf("unable to open file: %s\n", filepath);
        return -1;
    }

    printf("SJF Process List: ");

    printProcessList(process_list);
    runProcesses(process_list, &time, &k_factor);
    
    printf("Resultant process order: ");
    printProcessList(process_list);

}
