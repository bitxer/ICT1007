#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define GATE_COUNT_FNAME "gate.count"
#define VISITOR_COUNT_FNAME "visitor.count"
#define EXIT_COUNT_FNAME "exit.count"
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
		FILE * f_gateCount = fopen(GATE_COUNT_FNAME, "r+");
		FILE * f_visitorCount = fopen(VISITOR_COUNT_FNAME, "r+");
		FILE * f_exitCount = fopen(EXIT_COUNT_FNAME, "r+");

		fscanf(f_gateCount, "%d", &gateCount);
		fscanf(f_visitorCount, "%d", &visitorCount);
		fscanf(f_exitCount, "%d", &exitCount);

		fseek(f_gateCount, 0, SEEK_SET);
		fseek(f_visitorCount, 0, SEEK_SET);
		fseek(f_exitCount, 0, SEEK_SET);

		gateCount++;
		visitorCount++;
		exitCount++;

		fprintf(f_gateCount, "%d\n", gateCount);
		fprintf(f_visitorCount, "%d\n", visitorCount);
		fprintf(f_exitCount, "%d\n", exitCount);

		fclose(f_gateCount);
		fclose(f_visitorCount);
		fclose(f_exitCount);
	}
}

int main(int argc, char * argv[]) {
	// Check if program has 2 arguments
    // if (argc != 2) {
    //     printf("Usage: %s [token]\n", argv[0]);
    //     return 1;
    // }

	FILE * f_gateCount = fopen(GATE_COUNT_FNAME, "w");
	FILE * f_visitorCount = fopen(VISITOR_COUNT_FNAME, "w");
	FILE * f_exitCount = fopen(EXIT_COUNT_FNAME, "w");

	fprintf(f_gateCount, "%d\n", 0);
	fprintf(f_visitorCount, "%d\n", 0);
	fprintf(f_exitCount, "%d\n", 0);

	fclose(f_gateCount);
	fclose(f_visitorCount);
	fclose(f_exitCount);

    pthread_t t[5];
	for (int i = 0; i < 5; i++) {
        printf("Initialising Thread-%d\n", i);
        pthread_create(&t[i], NULL, gates, NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(t[i], NULL);
    }

	
	int gateCount, visitorCount, exitCount;
	f_gateCount = fopen(GATE_COUNT_FNAME, "r");
	f_visitorCount = fopen(VISITOR_COUNT_FNAME, "r");
	f_exitCount = fopen(EXIT_COUNT_FNAME, "r");

	fscanf(f_gateCount, "%d", &gateCount);
	fscanf(f_visitorCount, "%d", &visitorCount);
	fscanf(f_exitCount, "%d", &exitCount);

	fclose(f_gateCount);
	fclose(f_visitorCount);
	fclose(f_exitCount);

	printf("\n");
	printf("Gate count:\t%d\n", gateCount);
	printf("Vistor count:\t%d\n", visitorCount);
	printf("Exit count:\t%d\n", exitCount);
	return 0;
}