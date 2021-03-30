#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

// global mutex x5
pthread_mutex_t first_mutex, second_mutex, third_mutex, fourth_mutex, fifth_mutex;


// create a struct cause dynamic
typedef struct mutex_map_struct {
    int key;
    pthread_mutex_t *mutex;
    struct mutex_map_struct *next;
} mutex_map;


typedef struct thread_params_struct {
    mutex_map **map;
    mutex_map **head;
} thread_params;


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

void *cause_a_deadlock(void *param) {
    thread_params *tp = (thread_params*) param;
    mutex_map **map = tp->map;
    mutex_map **head = tp->head;
    
    int key = (*map)->key;
    pthread_mutex_t *mutex = (*map)->mutex;
    
    printf("Thread %d started...\n", key);

    // lock its own mutex
    pthread_mutex_lock(mutex);

    printf("Thread %d acquired mutex %d\n", key, key);

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
    printf("Thread %d attempting to acquire mutex %d...\n", key, mutex_num);
    pthread_mutex_lock(next_mutex_to_acquire);
    printf("Thread %d acquired mutex %d\n", key, mutex_num);
    // critical section
    printf("Inside Thread %d Critical Section\n", key);
    // end critical section

    pthread_mutex_unlock(mutex);
    pthread_mutex_unlock(next_mutex_to_acquire);
    printf("Thread %d released mutex %d and %d.\n", key, key, mutex_num);

    pthread_exit(0);
}

void traverseList(mutex_map** head) {
    mutex_map *temp = *head;
    while (temp != NULL) {
        printf("key: %d\n",temp->key);
        temp = temp->next;
    }
}

int main() {
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
    

    pthread_create(&thread_1, NULL, cause_a_deadlock, first_tp);
    pthread_create(&thread_2, NULL, cause_a_deadlock, second_tp);
    pthread_create(&thread_3, NULL, cause_a_deadlock, third_tp);
    pthread_create(&thread_4, NULL, cause_a_deadlock, fourth_tp);
    pthread_create(&thread_5, NULL, cause_a_deadlock, fifth_tp);

    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    pthread_join(thread_3, NULL);
    pthread_join(thread_4, NULL);
    pthread_join(thread_5, NULL);
}