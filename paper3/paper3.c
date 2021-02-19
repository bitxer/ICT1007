#include "paper3.h"

void insert_process(int process_no,int burst_time, int arrival_time){
    PROCESSNODE_PTR tempPtr = (PROCESSNODE_PTR) malloc (sizeof(PROCESSNODE));
    tempPtr->process_no = process_no;
    tempPtr->burst_time = burst_time;
    tempPtr->arrival_time = arrival_time;
    tempPtr->waiting_time = 0;
    tempPtr->turn_around_time = 0;

    if (master_head == NULL){
        master_head = tempPtr;
    } else {
        tempPtr->next = master_head;
        master_head = tempPtr;
    }
}

void print_list(PROCESSNODE_PTR list){
    PROCESSNODE_PTR current = list;
    float total_tat = 0;
    float total_wt = 0;
    printf("\nno.\tat\tbt\twt\ttat\n");
    while (current != NULL){
        printf("P%d\t%.2f\t%.2f\t%.2f\t%.2f\n", current->process_no, current->arrival_time,current->burst_time, current->waiting_time, current->turn_around_time);
        total_tat += current->turn_around_time;
        total_wt += current->waiting_time;
        current = current->next;
    }

    printf("Average Waiting Time: %.2f\nAverage Turn Around Time: %.2f\n", total_wt/no_process, total_tat/no_process);
}

void sort_by_arrival(){
    PROCESSNODE_PTR current, next;
    int temp_process_no, i, j,k;
    float temp_burst_time, temp_arrival_time, temp_waiting_time;

    k = no_process;
    for (i = 0; i < no_process - 1; i++,k--){
        current = master_head;
        next = current->next;

        for (j = 1; j < k; j++){
            //swap value with the current value and next value
            if ( current->arrival_time > next->arrival_time){
                temp_process_no = next->process_no;
                temp_arrival_time = next->arrival_time;
                temp_burst_time = next->burst_time;
                temp_waiting_time = next->waiting_time;
                
                next->process_no = current->process_no;
                next->arrival_time = current->arrival_time;
                next->burst_time = current->burst_time;
                next->temp_bt = current->burst_time;
                next->waiting_time = current->waiting_time;

                current->process_no = temp_process_no;
                current->burst_time = temp_burst_time;
                current->burst_time = temp_burst_time;
                current->arrival_time = temp_arrival_time;
                current->waiting_time = temp_waiting_time;
            }
            //move to next value
            current = current->next;
            next = next->next;
        }
    }
}

void start_process(){
    while (master_head != NULL){
        PROCESSNODE_PTR current = master_head;
        while (current != NULL){
            if ((current->arrival_time - temp_time_taken) <= 0){
                add_to_ready(current);
                master_head = current->next;
            }
            current = current->next;
        }

        if (ready_queue == NULL){
            float next_arrival_time = master_head->arrival_time;
            temp_time_taken = next_arrival_time;
        } else {
            sort_by_burst();
            split_to_small_heavy();
            round_robin(small_task_head);
            set_waiting_time(heavy_task_head);

            if (heavy_task_head != NULL){
            round_robin(heavy_task_head);
            }

            add_to_finish();
            update_waiting_times();

            small_task_head = NULL;
            heavy_task_head = NULL;
        }
    }
}

void add_to_ready(PROCESSNODE_PTR node){
    PROCESSNODE_PTR tempPtr = (PROCESSNODE_PTR) malloc (sizeof(PROCESSNODE));
    tempPtr->process_no = node->process_no;
    tempPtr->arrival_time = node->arrival_time;
    tempPtr->burst_time = node->burst_time;
    tempPtr->temp_bt = node->burst_time;
    tempPtr->waiting_time = node->waiting_time;
    
    if (ready_queue == NULL){
        ready_queue = tempPtr;
    } else {
        tempPtr->next = ready_queue;
        ready_queue = tempPtr;
    }
}

void add_to_finish(){
    if (finished_head == NULL){
        finished_head = small_task_head;
        finished_head = insert_node_at_end(finished_head, heavy_task_head);
    } else {
        finished_head = insert_node_at_end(finished_head, small_task_head);
        finished_head = insert_node_at_end(finished_head, heavy_task_head);
        //ready_queue->next = finished_head;
        //finished_head = ready_queue;
    }
}

void sort_by_burst(){
    PROCESSNODE_PTR current, next;
    int temp_process_no, i, j,k;
    float temp_burst_time, temp_arrival_time, temp_waiting_time;

    k = size_of_list(ready_queue);
    for (i = 0; i < no_process - 1; i++,k--){
        current = ready_queue;
        next = current->next;

        for (j = 1; j < k; j++){
            //swap value with the current value and next value
            if ( current->burst_time > next->burst_time){
                temp_process_no = next->process_no;
                temp_arrival_time = next->arrival_time;
                temp_burst_time = next->burst_time;
                temp_waiting_time = next->waiting_time;
                
                next->process_no = current->process_no;
                next->arrival_time = current->arrival_time;
                next->burst_time = current->burst_time;
                next->waiting_time = current->waiting_time;
                next->temp_bt = current->burst_time;

                current->process_no = temp_process_no;
                current->burst_time = temp_burst_time;
                current->temp_bt = temp_burst_time;
                current->arrival_time = temp_arrival_time;
                current->waiting_time = temp_waiting_time;

            }
            //move to next value
            current = current->next;
            next = next->next;
        }
    }
}

void split_to_small_heavy(){
    int list_size = size_of_list(ready_queue);
    int middle_node_no = (list_size + 1) / 2;
    for (int node_no = 1; node_no <= list_size; node_no++, ready_queue = ready_queue->next){
        PROCESSNODE_PTR tempPtr = (PROCESSNODE_PTR) malloc (sizeof(PROCESSNODE));
        tempPtr->process_no = ready_queue->process_no;
        tempPtr->arrival_time = ready_queue->arrival_time;
        tempPtr->burst_time = ready_queue->burst_time;
        tempPtr->temp_bt = ready_queue->temp_bt;
        tempPtr->waiting_time = ready_queue->waiting_time;
        if (node_no <= middle_node_no){
            small_task_head = insert_node_at_end(small_task_head, tempPtr);
        } else {
            heavy_task_head = insert_node_at_end(heavy_task_head, tempPtr);         
        }
    }
}

int size_of_list(PROCESSNODE_PTR node){
    PROCESSNODE_PTR tempPtr = node;
    int list_size = 0;
    while (tempPtr != NULL){
        list_size++;
        tempPtr = tempPtr->next;
    }
    return list_size;
}

PROCESSNODE_PTR insert_node_at_end(PROCESSNODE_PTR list,PROCESSNODE_PTR to_add){
    
    // Check if linklist exist
    if (list == NULL){
        list = to_add;
    } else {

        //Go to end of linklist
        PROCESSNODE_PTR temp = list;
        while (temp != NULL && temp->next != NULL){
            temp = temp->next;
        }

        //Add node to the end
        temp->next = to_add;
    }     
    return list;
}

void round_robin(PROCESSNODE_PTR queue){
    float time_quantum = get_time_quantum(queue);
    temp_time_taken += time_quantum * size_of_list(queue);

    float temp_burst_time = 0;
    float time_slice_used = 0;

    int process_complete = 0;

    //Continue RR until all temp_bt is 0
    do{    
        //Perform round robin on the queue
        PROCESSNODE_PTR current = queue;
        while (current != NULL){
            //To increase waiting time of processes in queue
            PROCESSNODE_PTR increase_wt = queue;


            temp_burst_time = current->temp_bt;
            temp_burst_time -= time_quantum;


            // Check if whole time slice is used up
            if (temp_burst_time < 0){

                // If whole time slice not used, set burst time = 0
                time_slice_used = current->temp_bt;
                current->turn_around_time += time_slice_used;
                current->temp_bt = 0;
            } else {
                // If whole time slice is used, reduce burst time by time slice
                current->temp_bt = temp_burst_time;
                time_slice_used = time_quantum;
            }
            if (current->temp_bt == 0){
                //current->waiting_time -= current->arrival_time;
                current->turn_around_time = current->waiting_time + current->burst_time;
                process_complete = 1;
            }
            //Increase wt of other processes
            while(increase_wt != NULL){
                if (!(current->process_no == increase_wt->process_no || increase_wt->temp_bt == 0)){
                    increase_wt->waiting_time += time_slice_used;
                }
                increase_wt = increase_wt->next;
            }
            current = current->next;
        }
        if (process_complete){
            time_quantum = get_time_quantum(queue);
        }
    } while(time_quantum != 0);

}

float get_time_quantum(PROCESSNODE_PTR queue){
    PROCESSNODE_PTR tempPtr = queue;
    float total_burst_time = 0, average_burst_time = 0;
    int queue_size = 0;

    //Sum the value of all burst time in linkedlist
    while (tempPtr != NULL) {
        if (tempPtr->temp_bt != 0){
        total_burst_time += tempPtr->temp_bt;
        queue_size++;
        }
        tempPtr = tempPtr->next;
    }

    //Calculate the average of burst time
    if (queue_size == 0){
        average_burst_time = 0;
    } else {
        average_burst_time = total_burst_time / queue_size;
    }
    return average_burst_time;
}

void set_waiting_time(PROCESSNODE_PTR queue){
    PROCESSNODE_PTR tempPtr = queue;
    while (tempPtr != NULL){
        tempPtr->waiting_time = temp_time_taken - tempPtr->arrival_time;
        tempPtr = tempPtr->next;
    }
}

void update_waiting_times(){
    PROCESSNODE_PTR tempPtr = master_head;
    while (tempPtr != NULL){
        if (temp_time_taken > tempPtr->arrival_time){
            tempPtr->waiting_time += temp_time_taken - tempPtr->arrival_time;
        }
        tempPtr = tempPtr->next;
    }
}