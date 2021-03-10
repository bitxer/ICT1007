#include "paper3.c"

/*
 * Main Loop.
 */
int main(){
    printf("Enter the no. of processes to: ");
    scanf("%d", &no_process);                   //Gets the number of process the program will run
    create_processes();
    sort_by_arrival();
    start_process();
    print_list(finished_head);                  //prints the end result of the program
}
/*
 * Creates process given arrival time and burst time
 * 
 * Saves the result in global variable master_head link list
 */
void create_processes(){
    float temp_burst_time, temp_arrival_time;                       // Declare temparory values to store burst and arrival time

    // Create <no_process> number of process and add it to linklist
    for (int counter = 1; counter <= no_process; counter++){
        printf("\nEnter Burst time for P%d: ", counter);
        scanf("%f", &temp_burst_time);                              //Get the burst time of the process
        printf("\nEnter Arrival time for P%d: ", counter); 
        scanf("%f", &temp_arrival_time);                            //Get the arrival time of the process
        insert_process(counter, temp_burst_time, temp_arrival_time);//Adds new process the queue
    }
}