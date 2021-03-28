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
    int s_dest = strlen(args->src) + 1 + strlen(args->dest) + 3;
    char * dest = malloc(s_dest);

    // Update destination with string
    snprintf(dest, s_dest, "%s/%s%d", args->dest, args->src, args->thread_id);
    pthread_mutex_lock(&mutex);
    printf("Thread-%d is copying files\n", args->thread_id);
    FILE * f_src = fopen(args->src, "rb");
    FILE * f_dst = fopen(dest, "w+");
    if (f_dst == NULL) {
        printf("Thread-%d: Error creating file %s\n", args->thread_id, dest);
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
    fclose(f_dst);
    fclose(f_src);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Usage: %s [source file] [destination_directory]\n", argv[0]);
        return 1;
    }

    if (access(argv[1], R_OK) != 0) {
        printf("\"%s\" is not readable\n", argv[1]);
        return 1;
    }

    struct stat st = {0};
    
    // Check if directory exist
    if (stat(argv[2], &st) == -1) {
        // Create if does not exist
        mkdir(argv[2], 0700);
    }


    //Initialize mutex
    pthread_mutex_init(&mutex, NULL);
    DEBUG("Mutex Initialised\n");
    pthread_t t[5];
    for (int i = 0; i < 5; i++) {
        printf("Initialising Thread-%d\n", i);
        // Initialising args
        ARGS_PTR args = malloc(sizeof(ARGS));

        // Initialise memory location
        int s_src = strlen(argv[1]) + 1, s_dest = strlen(argv[2]) + 1;
        args->src = malloc(s_src);
        args->dest = malloc(s_dest);
        
        // Setup arguments
        args->thread_id = i;
        snprintf(args->src, s_src, "%s", argv[1]);
        snprintf(args->dest, s_dest, "%s", argv[2]);

        // Create threads
        pthread_create(&t[i], NULL, copy, args);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(t[i], NULL);
    }

    //Destroy mutex after use
    pthread_mutex_destroy(&mutex);
    DEBUG("Mutex destroyed\n");
    // return 0;
}