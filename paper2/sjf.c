#include <stdio.h>
#include <stdlib.h>
#include "algo.h"

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

ProcessList* createTestData() {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    
    int process_id = 0;
    int arrival_time = 0;
    int burst_time = 0;

    ProcessList* head = NULL;

    fp = fopen("./data.csv", "r");
    if (fp == NULL)
        return NULL;

    while ((read = getline(&line, &len, fp)) != -1) {
        sscanf(line, "%d, %d, %d\n", &process_id, &burst_time, &arrival_time);
        ProcessList* newProc = malloc(sizeof(ProcessList));
        newProc->process_id = process_id;
        newProc->arrival_time = arrival_time;
        newProc->burst_time = burst_time;
        newProc->turnaround_time = 0;
        newProc->waiting_time = 0;
        newProc->next = NULL;

        sortedInsert(&head, newProc);
    }

    fclose(fp);
    if (line)
        free(line);
    
    return head;
}

void printList(ProcessList*  n) {
    while (n != NULL) {
        printf("P%d -> ", n->process_id);

        n = n->next;
    }
    printf("\n");
}


int main() {
    int k_factor = 2;
    ProcessList* head = NULL;
    head = createTestData();
    printList(head);

}
