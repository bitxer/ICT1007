#include "deadlock.h"

thread_params *createMutexMap(mutex_map** dict, int key, pthread_mutex_t* mutex, mutex_map** head) {
    int resp = pthread_mutex_init(mutex, NULL);
    assert(resp == 0);
    *dict = malloc(sizeof(struct mutex_map_struct));
    (*dict)->key = key;
    (*dict)->mutex = mutex;
    
    thread_params *tp = malloc(sizeof(struct thread_params_struct));
    tp->head = head;
    tp->map = dict;

    return tp;
}

void addToList(mutex_map** head, mutex_map** mutex) {
    if (*head == NULL) {
        *head = *mutex;
        return;
    }

    mutex_map* temp = *head;

    while (temp != NULL) {
        if (temp->next == NULL) {
            temp->next = *mutex;
            return;
        }
        temp = temp->next;
    }

}

void readFile(char** buffer, int key, int* filesize) {
    FILE *fp;

    char* filename = (char*)calloc(BUFFER_SIZE, sizeof(char));
    snprintf(filename, BUFFER_SIZE, "files/file_%d.txt", key);
    
    fp = fopen(filename, "r");

    // get size of contents in file
    fseek(fp, 0L, SEEK_END);
    *filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    if (*filesize > BUFFER_SIZE) {
        *buffer = realloc(*buffer, *filesize);
    }

    fread(*buffer, (*filesize)+1, 1, fp);
    free(filename);
    fclose(fp);
}

void writeFile(char** buffer, int key, int* filesize) {
    FILE *fp;

    char* filename = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int file_id = 0;
    
    if (key == NUMBER_OF_THREADS) {
        file_id = 1;
    } else {
        file_id = key+1;
    }
    snprintf(filename, BUFFER_SIZE, "files/file_%d.txt", file_id);
    
    fp = fopen(filename, "a");

    fwrite("\n", sizeof(char), 1, fp);
    fwrite(*buffer, *filesize, 1, fp);
    printf("[Process %d]\t Written content to ./%s\n", key, filename);

    free(filename);
    fclose(fp);
}

void *cause_a_deadlock(void *param) {
    thread_params *tp = (thread_params*) param;
    mutex_map **map = tp->map;
    mutex_map **head = tp->head;
    

    int key = (*map)->key;
    pthread_mutex_t *mutex = (*map)->mutex;
    
    printf("[Process %d]\t Process %d starting...\n", key, key);

    printf("[Process %d]\t Acquiring Mutex %d\n", key, key);

    // lock its own mutex
    pthread_mutex_lock(mutex);

    printf("[Process %d]\t Acquired Mutex %d\n", key, key);

    // find the next mutex
    mutex_map *temp = *head;

    while (temp != NULL) {
        if (temp->key == key) {
            break;
        }
        temp = temp->next;
    }

    pthread_mutex_t *next_mutex_to_acquire;
    int mutex_num = 0;

    if (temp->next != NULL) {
        mutex_num = temp->next->key;
        next_mutex_to_acquire = temp->next->mutex;

    } else {
        // means at the end of the linked list
        mutex_map *first = *head;
        next_mutex_to_acquire = first->mutex;
        mutex_num = first->key;
    }

    sleep(rand() % 10);
    printf("[Process %d]\t Acquiring Mutex %d...\n", key, mutex_num);
    pthread_mutex_lock(next_mutex_to_acquire);
    printf("[Process %d]\t Acquired Mutex %d\n", key, mutex_num);

    // critical section
    printf("[Process %d]\t Entering Critical Section...\n", key);

    char* readBuffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int filesize = 0;

    readFile(&readBuffer, key, &filesize);

    printf("[Process %d]\t Contents of file_%d: %s\n", key, key, readBuffer);

    writeFile(&readBuffer, key, &filesize);

    // end critical section

    pthread_mutex_unlock(mutex);
    pthread_mutex_unlock(next_mutex_to_acquire);
    printf("[Process %d]\t Released Mutex %d and %d.\n", key, key, mutex_num);

    pthread_exit(0);
}

void *fix_a_deadlock(void *param) {
    thread_params *tp = (thread_params*) param;
    mutex_map **map = tp->map;
    mutex_map **head = tp->head;
    

    int key = (*map)->key;
    pthread_mutex_t *mutex = (*map)->mutex;
    
    printf("[Process %d]\t Process %d starting... \n", key, key);

    printf("[Process %d]\t Acquiring Mutex %d...\n", key, key);
    // lock its own mutex
    pthread_mutex_lock(mutex);

    printf("[Process %d]\t Acquired Mutex %d\n", key, key);

    // critical section 1
    printf("[Process %d]\t Entering Critical Section 1\n", key);
    char *readBuffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int filesize = 0;
    readFile(&readBuffer, key, &filesize);

    printf("[Process %d]\t Contents of file_%d: %s\n", key, key, readBuffer);

    // end critical section 1
    printf("[Process %d]\t End of Critical Section 1\n", key);

    // unlock the mutex its already holding
    pthread_mutex_unlock(mutex);
    printf("[Process %d]\t Released Mutex %d\n", key, key);

    // find the next mutex
    mutex_map *temp = *head;

    while (temp != NULL) {
        if (temp->key == key) {
            break;
        }
        temp = temp->next;
    }

    pthread_mutex_t *next_mutex_to_acquire;
    int mutex_num = 0;

    if (temp->next != NULL) {
        mutex_num = temp->next->key;
        next_mutex_to_acquire = temp->next->mutex;

    } else {
        // means at the end of the linked list
        mutex_map *first = *head;
        next_mutex_to_acquire = first->mutex;
        mutex_num = first->key;
    }

    sleep(rand() % 10);
    printf("[Process %d]\t Acquiring Mutex %d...\n", key, mutex_num);
    pthread_mutex_lock(next_mutex_to_acquire);
    printf("[Process %d]\t Acquired Mutex %d\n", key, mutex_num);

    // critical section
    printf("[Process %d]\t Entering Critical Section 2\n", key);
    writeFile(&readBuffer, key, &filesize);
    // end critical section

    pthread_mutex_unlock(next_mutex_to_acquire);
    printf("[Process %d]\t Released Mutex %d\n", key, mutex_num);

    pthread_exit(0);
}

void traverseList(mutex_map** head) {
    mutex_map *temp = *head;
    while (temp != NULL) {
        printf("key: %d\n",temp->key);
        temp = temp->next;
    }
}

/*
     printHelp
    prints the help menu
*/
void printHelp(const char* name, const char* option) {
    printf("===========================================\n");
    printf("DEADLOCK\n");
    printf("This program simulates and solves deadlock.\n\n");
    printf("The program's objective is to create \nN number of threads.\n\n"); 
    printf("Each thread will read file_n.txt and will \nappend the contents ");
    printf("over to file_n+1.txt.\n");
    printf("===========================================\n");
    printf("usage:\t%s [flags]\n", name);
    printf("-d\tdeadlock mode\n");
    printf("-h\tprint this help menu\n");
    printf("-s\tsolve the deadlock\n");
    return;
}

int main(int argc, char* argv[]) {
    int opt = 0;
    void *function_to_run = NULL;
    
    if (argc > 1) {
        while ((opt = getopt(argc, argv, "dhs")) != -1) {
            switch (opt) {
                case 'd':
                    function_to_run = cause_a_deadlock;
                    break;
                case 'h':
                    printHelp(argv[0], argv[1]);
                    return 0;
                case 's':
                    // solution mode
                    function_to_run = fix_a_deadlock;
                    break;
                default: /* '?' */
                    printHelp(argv[0], argv[1]);
                    return -1;
            }
        }
    } else {
        printHelp(argv[0], argv[1]);
        return -1;
    }

    srand(time(0));

    pthread_t thread_1, thread_2, thread_3, thread_4, thread_5;
    
    mutex_map *first_mutex_map, *second_mutex_map, *third_mutex_map, *fourth_mutex_map, *fifth_mutex_map;
    mutex_map *head = NULL;
    

    thread_params *first_tp = createMutexMap(&first_mutex_map, 1, &first_mutex, &head);
    thread_params *second_tp = createMutexMap(&second_mutex_map, 2, &second_mutex, &head);
    thread_params *third_tp = createMutexMap(&third_mutex_map, 3, &third_mutex, &head);
    thread_params *fourth_tp = createMutexMap(&fourth_mutex_map, 4, &fourth_mutex, &head);
    thread_params *fifth_tp = createMutexMap(&fifth_mutex_map, 5, &fifth_mutex, &head);

    addToList(&head, &first_mutex_map);
    addToList(&head, &second_mutex_map);
    addToList(&head, &third_mutex_map);
    addToList(&head, &fourth_mutex_map);
    addToList(&head, &fifth_mutex_map);
    

    pthread_create(&thread_1, NULL, function_to_run, first_tp);
    pthread_create(&thread_2, NULL, function_to_run, second_tp);
    pthread_create(&thread_3, NULL, function_to_run, third_tp);
    pthread_create(&thread_4, NULL, function_to_run, fourth_tp);
    pthread_create(&thread_5, NULL, function_to_run, fifth_tp);

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    pthread_join(thread_3, NULL);
    pthread_join(thread_4, NULL);
    pthread_join(thread_5, NULL);
}