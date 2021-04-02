#include "starve.h"

/*
 * Multi threaded copy
 * 
 * Parameters:
 *   _args  - Argument for function
 */
void * copy(void * _args) {
    // Initialise arguments
    ARGS_PTR args = (ARGS_PTR) _args;
    
    // Intiliase buffer
    char buffer[10];

    // Take note of current time to indicate start of current thread execution
    time_t start = time(NULL);
    pthread_mutex_lock(&print_lock);
    printf("Thread-%d initialised\n", args->thread_id);
    pthread_mutex_unlock(&print_lock);
    if (args->thread_id > 99) {
        // Check if thread id is valid
        pthread_exit(NULL);
    }
    
    pthread_mutex_lock(&print_lock);
    printf("Thread-%d is copying files\n", args->thread_id);
    pthread_mutex_unlock(&print_lock);

    // Open files for reading
    FILE * f_src = fopen(args->src, "rb");
    FILE * f_dst = fopen(args->dest, "w+");
    
    // Check if destination file is opened
    if (f_dst == NULL) {
        // Handle if destination file cannot be opened
        pthread_mutex_lock(&print_lock);
        printf("Thread-%d: Error creating file %s\n", args->thread_id, args->dest);
        pthread_mutex_unlock(&print_lock);
        pthread_exit(NULL);
    }

    // Copy file contents
    int nread = 0;
    while (nread = fread(buffer, 1, sizeof(buffer), f_src) ) {
        fwrite(buffer, 1, nread, f_dst);
    }

    // Close file
    fclose(f_dst);
    fclose(f_src);

    // Take note of current time to indicate start of current thread execution
    time_t end = time(NULL);

    pthread_mutex_lock(&print_lock);
    printf("Thread-%d completed\n", args->thread_id);
    pthread_mutex_unlock(&print_lock);

    // Execution time of current thread
    args->t_execution = end - start;
    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
    // Check if program has 3 arguments
    if (argc != 3) {
        printf("Usage: %s [source file] [destination_directory]\n", argv[0]);
        return 1;
    }

    // Check if file can be opened for reading
    if (access(argv[1], R_OK) != 0) {
        printf("\"%s\" is not readable\n", argv[1]);
        return 1;
    }

    // Check if directory exist
    struct stat st = {0};
    if (stat(argv[2], &st) == -1) {
        // Create if does not exist
        mkdir(argv[2], 0700);
    }

    // Get file name from src in case file is a path
    char * f_name = basename(argv[1]);
    int s_src = strlen(argv[1]) + 1;
    int s_dest = strlen(f_name) + 1 + strlen(argv[2]) + 3;

    //Initialize variables for use in threads
    pthread_mutex_init(&print_lock, NULL);
    pthread_t t[5];
    ARGS_PTR args[5];

    // Start of actual execution
    time_t start = time(NULL);
    for (int i = 0; i < 5; i++) {
        printf("Initialising Thread-%d\n", i + 1);
        // Initialising args
        args[i] = malloc(sizeof(ARGS));

        // Initialise memory location
        args[i]->src = malloc(s_src);
        args[i]->dest = malloc(s_dest);
        
        // Setup arguments
        args[i]->thread_id = i + 1;
        snprintf(args[i]->src, s_src, "%s", argv[1]);
        snprintf(args[i]->dest, s_dest, "%s/%s%d", argv[2], f_name, i + 1);

        // Create threads
        pthread_create(&t[i], NULL, copy, args[i]);
    }

    // Wait for all threads to finish execution
    for (int i = 0; i < 5; i++) {
        pthread_join(t[i], NULL);
    }

    // End of execution for all threads
    time_t end = time(NULL);

    // Compute total execution time for all threads
    int t_execution = end - start;

    // Print execution time for individual threads
    printf("\n---- Time taken for each thread ---\n");
    for (int i = 0; i < 5; i++) {
        printf("Thread-%d took %ds to run\n", args[i]->thread_id, args[i]->t_execution);
    }

    // Print total execution time
    printf("\nTotal execution time: %ds\n", t_execution);
    
    // Destroy mutex after use
    pthread_mutex_destroy(&print_lock);
    return 0;
}