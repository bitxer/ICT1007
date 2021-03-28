#include "starve.h"

void * copy(void * _args) {
    // Initialise arguments
    ARGS_PTR args = (ARGS_PTR) _args;
    
    printf("Thread-%d initialised\n", args->thread_id);
    if (args->thread_id > 99) {
        // Check if thread id is valid
        pthread_exit(NULL);
    }
    
    // Initialise memory location for destination
    int s_dest = strlen(args->src) + 1 + strlen(args->dest) + 2;
    char * dest = malloc(s_dest);

    // Update destination with string
    snprintf(dest, s_dest, "%s/%s%d", args->src, args->dest, args->thread_id);
    pthread_mutex_lock(&mutex);
    printf("Thread-%d is copying files\n", args->thread_id);
    sleep(30);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Usage: %s [source file] [destination_directory]\n", argv[0]);
        return 1;
    }

    //Initialize mutex
    pthread_mutex_init(&mutex, NULL);
    DEBUG("Mutex Initialised\n");
    pthread_t t[5];
    for (int i = 1; i < 6; i++) {
        printf("Initialising Thread-%d\n", i);
        // Initialising args
        ARGS_PTR args = malloc(sizeof(ARGS));

        // Initialise memory location
        args->src = malloc(strlen(argv[1]) + 1);
        args->dest = malloc(strlen(argv[2]) + 1);
        
        // Setup arguments
        args->thread_id = i;
        snprintf(args->src, strlen(argv[1]) + 1, "%s", argv[1]);
        snprintf(args->dest, strlen(argv[2]) + 1, "%s", argv[2]);

        // Create threads
        pthread_create(&t[i], NULL, copy, args);
    }
    for (int i = 1; i < 6; i++) {
        pthread_join(t[i], NULL);
    }

    //Destroy mutex after use
    pthread_mutex_destroy(&mutex);
    DEBUG("Mutex destroyed\n");
    return 0;
}