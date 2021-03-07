#include "paper3.h"

/*
 *  Insert ProcessNode into ProcessNode Queue in master_head
 * 
 *  Parameters:
 *      process_no   -   PID of process
 *      burst_time   -   burst time of the process
 *      arrival_time -   arrival time of the process
 * 
 */
void insert_process(int process_no,int burst_time, int arrival_time){

    //Intialise struct values
    PROCESSNODE_PTR tempPtr = (PROCESSNODE_PTR) malloc (sizeof(PROCESSNODE));
    tempPtr->process_no = process_no;
    tempPtr->burst_time = burst_time;
    tempPtr->arrival_time = arrival_time;
    tempPtr->waiting_time = 0;
    tempPtr->turn_around_time = 0;

    if (master_head == NULL){       //Check if master head is not created
        master_head = tempPtr;      //Sets master head to newly created process
    } else {
        tempPtr->next = master_head;//Adds a new process to the master_head
        master_head = tempPtr;
    }
}

/*
 *  Prints the total arrival time, waiting time, turn around time of all process in the queue
 * 
 *  Parameters:
 *      list    - a linklist containig the PROCESSNODE struct
 *  
 */
void print_list(PROCESSNODE_PTR list){
    //create a temp value for pointer
    PROCESSNODE_PTR current = list;

    //intialise value for calculating average turn around time and waiting time
    float total_tat = 0;
    float total_wt = 0;
    printf("\nno.\tat\tbt\twt\ttat\n");

    //Prints all values in the linkedlist
    while (current != NULL){
        printf("P%d\t%.2f\t%.2f\t%.2f\t%.2f\n", current->process_no, current->arrival_time,current->burst_time, current->waiting_time, current->turn_around_time);
        total_tat += current->turn_around_time;
        total_wt += current->waiting_time;
        current = current->next;
    }
    //prints avrage waiting time and tunr around time
    printf("Average Waiting Time: %.2f\nAverage Turn Around Time: %.2f\n", total_wt/no_process, total_tat/no_process);
}

/*
 *  Sorts the master head according to arrival time
 * 
 */
void sort_by_arrival(){
    //initialise values to perform bubble sort
    PROCESSNODE_PTR current, next;
    int temp_process_no, i, j,k;
    float temp_burst_time, temp_arrival_time, temp_waiting_time;

    k = no_process;

    //Performs bubble sort on the linklist

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
/*
 *  Starts the algorithm process
 * 
 */ 
void start_process(){
    //Function will end when master_head is empty
    while (master_head != NULL){
        PROCESSNODE_PTR current = master_head;

        //Add values to ready queue
        while (current != NULL){

            //enters this block if arrival time is less than cpu time
            if ((current->arrival_time - temp_time_taken) <= 0){
                add_to_ready(current);
                master_head = current->next;
            }
            current = current->next;
        }
        if (ready_queue == NULL){         //enter this block if no processes is added to ready queue
            //Gets the next arrival time of the process and updates the cpu time to be the next arrival time
            float next_arrival_time = master_head->arrival_time;
            temp_time_taken = next_arrival_time;
        } else {        //enter this block if ready queue is created succesfully
            sort_by_burst();
            split_to_small_heavy();
            round_robin(small_task_head);
            set_waiting_time(heavy_task_head);

            //Check if heavy queue exist to perform round robin
            if (heavy_task_head != NULL){
            round_robin(heavy_task_head);
            }

            add_to_finish();
            update_waiting_times();
            //resets the pointers of small and heavy queue
            small_task_head = NULL;
            heavy_task_head = NULL;
        }
    }
}
/*
 *  Adds the current node to a ready queue
 *  
 *  Parameters:
 *      node    -   PROCESSNODE struct to be added to ready queue
 *  
 */
void add_to_ready(PROCESSNODE_PTR node){
    //initialise temp variables
    PROCESSNODE_PTR tempPtr = (PROCESSNODE_PTR) malloc (sizeof(PROCESSNODE));
    tempPtr->process_no = node->process_no;
    tempPtr->arrival_time = node->arrival_time;
    tempPtr->burst_time = node->burst_time;
    tempPtr->temp_bt = node->burst_time;
    tempPtr->waiting_time = node->waiting_time;
    
    if (ready_queue == NULL){   //enter this block if ready queue is not created yet
        ready_queue = tempPtr;  //sets the node to be the ready queue
    } else {                    //enter this block if ready queue is found
        tempPtr->next = ready_queue;    //adds the node to the ready queue
        ready_queue = tempPtr;
    }
}
/*
 *  Adds the finish process to a finish linklist
 * 
 */
void add_to_finish(){
    if (finished_head == NULL){//enter this block if finished linklist is not created yet
        finished_head = small_task_head;//sets the linklist to be small head list
        finished_head = insert_node_at_end(finished_head, heavy_task_head); //Appends the heavy list to the end of linklist
    } else {
        finished_head = insert_node_at_end(finished_head, small_task_head); //Appends small queue to end of linklist
        finished_head = insert_node_at_end(finished_head, heavy_task_head); //Appends heavy queue to end of linklist
    }
}

/**
 *  Sorts the linklist by its burst time
 * 
 */
void sort_by_burst(){
    //initialise value for sorting
    PROCESSNODE_PTR current, next;
    int temp_process_no, i, j,k;
    float temp_burst_time, temp_arrival_time, temp_waiting_time;
    //get the size of ready_queue
    k = size_of_list(ready_queue);

    //perform bubble sort on linklist
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

/**
 *  Splits to queue into half where the 1st half will be the small queue, the 2nd half be the heavy queue
 *  The middle node will be part of the small queue
 * 
 */
void split_to_small_heavy(){
    //gets the size of the quwuw
    int list_size = size_of_list(ready_queue);

    //get the middle node index
    int middle_node_no = (list_size + 1) / 2;

    //separates the nodes into small and heavy queue
    for (int node_no = 1; node_no <= list_size; node_no++, ready_queue = ready_queue->next){
        PROCESSNODE_PTR tempPtr = (PROCESSNODE_PTR) malloc (sizeof(PROCESSNODE));
        tempPtr->process_no = ready_queue->process_no;
        tempPtr->arrival_time = ready_queue->arrival_time;
        tempPtr->burst_time = ready_queue->burst_time;
        tempPtr->temp_bt = ready_queue->temp_bt;
        tempPtr->waiting_time = ready_queue->waiting_time;
        if (node_no <= middle_node_no){//If the index is less than or equal middle node index 
            small_task_head = insert_node_at_end(small_task_head, tempPtr); //add the node to small queue
        } else {
            heavy_task_head = insert_node_at_end(heavy_task_head, tempPtr); //add the node to heavy queue
        }
    }
}

/**
 *  Returns the size of the linklist
 * 
 *  Parameter:
 *      node    - the linklist to find the length of
 * 
 *  Return:
 *      list_size   - size if the linklist
 */
int size_of_list(PROCESSNODE_PTR node){
    PROCESSNODE_PTR tempPtr = node;
    int list_size = 0;

    //counts the number of nodes
    while (tempPtr != NULL){
        list_size++;
        tempPtr = tempPtr->next;
    }
    return list_size;
}

/**
 *  Adds the node to the end of the linklist
 * 
 *  Parameter:
 *      list    - the linklist to add to node to
 *      to_add  - the node to attach to list
 * 
 *  Return:
 *      list    - the end result of the combined linklist
 * 
 */ 
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

/**
 *  Round Robin function
 * 
 *  Parameter:
 *      queue   - the queue to perform Round Robin on
 * 
 */
void round_robin(PROCESSNODE_PTR queue){

    //Get the time quantum of the current queue
    float time_quantum = get_time_quantum(queue);

    //update the cpu time
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
                current->turn_around_time = current->waiting_time + current->burst_time;
                process_complete = 1;
            }
            //Increase waiting time of other processes
            while(increase_wt != NULL){
                if (!(current->process_no == increase_wt->process_no || increase_wt->temp_bt == 0)){
                    increase_wt->waiting_time += time_slice_used;
                }
                increase_wt = increase_wt->next;
            }
            current = current->next;
        }
        //Checks if the process is completed
        if (process_complete){
            time_quantum = get_time_quantum(queue); //update the current time quantum
        }
    } while(time_quantum != 0);

}

/**
 *  Calculates the time quantum of the queue
 * 
 *  Parameter:
 *      queue   - the queue the calculate time quantum from
 * 
 *  Return:
 *      average_burst_time  - returns the avergae burst time which is the time quatum for this algorithm
 */
float get_time_quantum(PROCESSNODE_PTR queue){
    PROCESSNODE_PTR tempPtr = queue;
    float total_burst_time = 0, average_burst_time = 0;
    int queue_size = 0;

    //Sum the value of all burst time in linkedlist
    while (tempPtr != NULL) {
        //check if the process is not finish
        if (tempPtr->temp_bt != 0){
        total_burst_time += tempPtr->temp_bt;
        queue_size++;
        }
        tempPtr = tempPtr->next;
    }

    //Calculate the average of burst time
    if (queue_size == 0){
        average_burst_time = 0; //to prevent division by zero error
    } else {
        average_burst_time = total_burst_time / queue_size; 
    }
    return average_burst_time;
}

/**
 *  Adds the waiting time of all processes in the heavy queue
 * 
 *  Parameter:
 *      queue   -   the processes in queue
 * 
 */
void set_waiting_time(PROCESSNODE_PTR queue){
    PROCESSNODE_PTR tempPtr = queue;
    while (tempPtr != NULL){
        //sets the waiting time of the process
        tempPtr->waiting_time = temp_time_taken - tempPtr->arrival_time;
        tempPtr = tempPtr->next;
    }
}

/**
 *  Updates the waiting time of processes that are left in master_head
 */
void update_waiting_times(){
    PROCESSNODE_PTR tempPtr = master_head;
    while (tempPtr != NULL){
        //Check if cpu time is more than the process arrival time
        if (temp_time_taken > tempPtr->arrival_time){
            //increase the waiting time of processes in the queue
            tempPtr->waiting_time += temp_time_taken - tempPtr->arrival_time;
        }
        tempPtr = tempPtr->next;
    }
}