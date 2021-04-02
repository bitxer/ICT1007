#include "deadlock.h"

/*
    createMutexMap()
    Initializes the mutex and uses the key to create a Mutex Map structure
    This also takes in the head to create the linked list for later
    ---------------
    Parameters
    ---------------
    mutex_map** dict: [pass by reference] the pointer of the mutex_map struct pointer so that you can create the dict (it's a dict cause its effectively a dictionary structure)
    int key: the number ID to be assigned to this particular Mutex, to be put into the mutex map
    pthread_mutex_t*: pointer to the pthread_mutex_t object, to be put into the mutex map
    mutex_map** head: [pass by reference] the pointer to the mutex_map struct pointer in order to create a linked list for later
    Returns the thread_param struct so that both the mutex map and the dict can be passed into the pthread_create() function
*/
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

/*
    addToList()
    Adds a given Mutex Map into the linked list
    ---------------
    Parameters
    ---------------
    mutex_map** head: head of the linked list
    mutex_map** mutex: the mutex to add to the linked list
*/
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

/*
    readFile()
    opens the file in ./files/file_<key>.txt, and reads the file contents into the buffer and stores the file size into the filesize variable
    ---------------
    Parameters
    ---------------
    char** buffer: pointer to the character pointer buffer that will hold the contents of the file
    int key: the file number to read
    int* filesize: the pointer to the int that will hold the filesize
*/
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

/*
    writeFile()
    opens the file in ./files/file_<key>.txt, and writes the given content into the new line
    ---------------
    Parameters
    ---------------
    char** buffer: pointer to the character pointer buffer that will hold the contents of the file
    int key: the file number to write to
    int* filesize: the pointer to the int that will hold the filesize
*/
void writeFile(char** buffer, int key, int* filesize) {
    FILE *fp;

    char* filename = (char*)calloc(BUFFER_SIZE, sizeof(char));
    int file_id = 0;
    
    // increments the key, because we want to write to the NEXT file
    // checks if its the last thread, if it is, loop back to 1
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

/*
    cause_a_deadlock()
    Obviously, this function will cause a deadlock, by holding on to the mutex while requesting for another (hopefully locked) mutex
    ---------------
    Parameters
    ---------------
    void* param: pointer to the thread_params*
*/
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

/*
    fix_a_deadlock()
    Again, this function will NOT cause a deadlock, but instead, fix the problem by releasing on to the mutex before requesting for another (hopefully locked) mutex
    ---------------
    Parameters
    ---------------
    void* param: pointer to the thread_params*
*/
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

/*
    traverseList()
    Originally for testing purposes, it's just to traverse and print out the key of each mutex map in the linked list
    ---------------
    Parameters
    ---------------
    mutex_map** head: the head of the linked list
*/
void traverseList(mutex_map** head) {
    mutex_map *temp = *head;
    while (temp != NULL) {
        printf("key: %d\n",temp->key);
        temp = temp->next;
    }
}

/*
    printHelp()
    prints the help menu
    ---------------
    Parameters
    ---------------
    const char* name: the name of the program, usually argv[0]
*/
void printHelp(const char* name) {
    printf("===========================================\n");
    printf("DEADLOCK\n");
    printf("This program simulates and solves deadlock.\n\n");
    printf("The program's objective is to create \nN number of threads.\n\n"); 
    printf("Each thread will read file_n.txt and will \nappend the contents ");
    printf("over to file_n+1.txt.\n");
    printf("===========================================\n");
    printf("usage:\t%s [flags] [-n <number of processes>]\n", name);
    printf("-d\tdeadlock mode\n");
    printf("-h\tprint this help menu\n");
    printf("-n\tnumber of processes to create <insert number>\n");
    printf("-s\tsolve the deadlock\n");

    return;
}

/*
    main()
    who doesn't love the main? very standard
*/
int main(int argc, char* argv[]) {
    int opt = 0;
    void *function_to_run = NULL;
    NUMBER_OF_THREADS = 5;
    
    if (argc > 1) {
        while ((opt = getopt(argc, argv, "dhsn:")) != -1) {
            switch (opt) {
                case 'd':
                    function_to_run = cause_a_deadlock;
                    break;
                case 'h':
                    printHelp(argv[0]);
                    return 0;
                case 's':
                    // solution mode
                    function_to_run = fix_a_deadlock;
                    break;
                case 'n':
                    NUMBER_OF_THREADS = atoi(optarg);
                    if (NUMBER_OF_THREADS == 0) {
                        printHelp(argv[0]);
                        return -1;
                    } else {
                        break;
                    }
                default: /* '?' */
                    printHelp(argv[0]);
                    return -1;
            }
        }
    } else {
        printHelp(argv[0]);
        return -1;
    }

    srand(time(0));

    pthread_mutex_t mutex_array[NUMBER_OF_THREADS];
    pthread_t thread_array[NUMBER_OF_THREADS];
    thread_params *thread_params_array[NUMBER_OF_THREADS];
    mutex_map* mutex_map_array[NUMBER_OF_THREADS];
    mutex_map* head = NULL;

    for (int i=0; i<NUMBER_OF_THREADS; i++) {
        thread_params* tp = createMutexMap(&mutex_map_array[i], i+1, &mutex_array[i], &head);
        addToList(&head, &mutex_map_array[i]);
        thread_params_array[i] = tp;
    }

    for (int i=0; i<NUMBER_OF_THREADS; i++) {
        pthread_create(&thread_array[i], NULL, function_to_run, thread_params_array[i]);
    }

    for (int i=0; i<NUMBER_OF_THREADS; i++) {
        pthread_join(thread_array[i], NULL);
    }
}