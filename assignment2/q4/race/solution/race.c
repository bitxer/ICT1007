/*
 * Written by: YongZe (@bitxer)
 */

#include <stdio.h>
#include <pthread.h>

// Initialise 
#define GATE_COUNT_FNAME "gate.count"
#define VISITOR_COUNT_FNAME "visitor.count"
#define EXIT_COUNT_FNAME "exit.count"

pthread_mutex_t mutex;

/*
 * Scenerio: 5 threads to act as 5 gates to a compound
 * 3 files is used to keep track of the number of times the gates was opened
 * They are gate.count, visitor.count and exit.count
 * At the end of the program execution, the gate is opened 1000 times
 * Everytime the gate opens, 1 visitor enters and 1 visitor exits
 * At the end of the execution, all files should have the same value of 5000
 */
void * gates(void * _args) {
	for (int i = 0; i < 1000; i++) {
		int gateCount = 0, visitorCount = 0, exitCount = 0;
		// Acquire mutex lock
    	pthread_mutex_lock(&mutex);

		// Open file for reading and writing
		FILE * f_gateCount = fopen(GATE_COUNT_FNAME, "r+");
		FILE * f_visitorCount = fopen(VISITOR_COUNT_FNAME, "r+");
		FILE * f_exitCount = fopen(EXIT_COUNT_FNAME, "r+");

		// Read counts from file
		fscanf(f_gateCount, "%d", &gateCount);
		fscanf(f_visitorCount, "%d", &visitorCount);
		fscanf(f_exitCount, "%d", &exitCount);

		// Increment all counts
		gateCount++;
		visitorCount++;
		exitCount++;

		// Reset to start of files
		fseek(f_gateCount, 0, SEEK_SET);
		fseek(f_visitorCount, 0, SEEK_SET);
		fseek(f_exitCount, 0, SEEK_SET);

		// Write updated values to file
		fprintf(f_gateCount, "%d\n", gateCount);
		fprintf(f_visitorCount, "%d\n", visitorCount);
		fprintf(f_exitCount, "%d\n", exitCount);

		// Close file
		fclose(f_gateCount);
		fclose(f_visitorCount);
		fclose(f_exitCount);

		// Release mutex lock
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, char * argv[]) {
	// Creates file to track gate count
	FILE * f_gateCount = fopen(GATE_COUNT_FNAME, "w");
	fprintf(f_gateCount, "%d\n", 0);
	fclose(f_gateCount);

	// Creates file to track visitor entry count
	FILE * f_visitorCount = fopen(VISITOR_COUNT_FNAME, "w");
	fprintf(f_visitorCount, "%d\n", 0);
	fclose(f_visitorCount);

	// Creates file to track visitor exit count
	FILE * f_exitCount = fopen(EXIT_COUNT_FNAME, "w");
	fprintf(f_exitCount, "%d\n", 0);
	fclose(f_exitCount);

    //Initialize mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_t t[5];

	// Initialise 5 threads to simulate 5 gates
	for (int i = 0; i < 5; i++) {
        printf("Initialising Thread-%d\n", i);
        pthread_create(&t[i], NULL, gates, NULL);
    }

	// Wait for program to finish execution
    for (int i = 0; i < 5; i++) {
        pthread_join(t[i], NULL);
    }

	// Initialise variables for the different count
	int gateCount, visitorCount, exitCount;

	// Read gate count from file
	f_gateCount = fopen(GATE_COUNT_FNAME, "r");
	fscanf(f_gateCount, "%d", &gateCount);
	fclose(f_gateCount);

	// Read visitor entry count from file
	f_visitorCount = fopen(VISITOR_COUNT_FNAME, "r");
	fscanf(f_visitorCount, "%d", &visitorCount);
	fclose(f_visitorCount);

	// Read visitor exit count from file
	f_exitCount = fopen(EXIT_COUNT_FNAME, "r");
	fscanf(f_exitCount, "%d", &exitCount);
	fclose(f_exitCount);

    // Destroy mutex after use
    pthread_mutex_destroy(&mutex);

	// Print details of current execution
	printf("\n");
	printf("Gate count:\t%d\n", gateCount);
	printf("Vistor count:\t%d\n", visitorCount);
	printf("Exit count:\t%d\n", exitCount);
	return 0;
}