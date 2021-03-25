#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
	FILE *f;
	printf("sanity\n");
        f = fopen("temp.txt", "a");
	if (f == NULL) {
		printf("ERROR\n");
		return 1;
	} else {
		printf("FILE POINTER AQUIRED\n");
	}
	for (int i = 0; i <= 10000; i++) {
		fprintf(f, "%s\n", argv[1]);
		sleep(0.5);
	}
	fclose(f);
	return 0;

	
}