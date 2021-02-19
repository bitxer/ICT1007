#include "paper3.c"

/*
 * Main Loop.
 */
int main(){
    printf("Enter the no. of processes to: ");
    scanf("%d", &no_process);
    create_processes();
    sort_by_arrival();
    start_process();
    print_list(finished_head);
}
/*
 * Creates process given arrival time and burst time
 * 
 * Saves the result in global variable master_head link list
 */
void create_processes(){
    float temp_burst_time, temp_arrival_time;

    // Create <no_process> number of process and add it to linklist
    for (int counter = 1; counter <= no_process; counter++){
        printf("\nEnter Burst time for P%d: ", counter);
        scanf("%f", &temp_burst_time);
        printf("\nEnter Arrival time for P%d: ", counter);
        scanf("%f", &temp_arrival_time);
        insert_process(counter, temp_burst_time, temp_arrival_time);
    }
}