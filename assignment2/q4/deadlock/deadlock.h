#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

#define BUFFER_SIZE 256
#define NUMBER_OF_THREADS 5

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

// function prototypes
thread_params *createMutexMap(mutex_map** dict, int key, pthread_mutex_t* mutex, mutex_map** head);
void addToList(mutex_map** head, mutex_map** mutex);
void readFile(char** buffer, int key, int* filesize);
void writeFile(char** buffer, int key, int* filesize);
void *cause_a_deadlock(void *param);
void *fix_a_deadlock(void *param);
void traverseList(mutex_map** head);
void printHelp(const char* name, const char* option);
int main(int argc, char* argv[]);

