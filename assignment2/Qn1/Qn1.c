#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/times.h>


int verify_child_processes(int pid, int number_of_processes, pid_t pids[]);
void create_child_processes(int counter, int argc, pid_t pid, char *argv[], pid_t process[], char *arguments[]);
void wait_processes(int child, int status, int argc, pid_t process[], char *argv[], struct tms tms_start, struct tms tms_end);

int main(int argc, char *argv[]) {

	pid_t pid, process[argc];
	int counter, child, status;
	struct tms tms_start, tms_end;
	char *arguments[100];

	/*The program ensures that the user enters at least a single command. The first argc belongs to the name of the program
	  and the second argc belongs to the command. It will exit if it does not match the criteria. */
	if (argc < 2) {
		fprintf(stderr, "Usage: ./Qn1 [COMMAND] [COMMAND]...[COMMAND]\n");
		fprintf(stderr, "\tThis program takes a list of command line arguments and would create N direct\n");
		fprintf(stderr,	"\tchild processes.Each child executing one of the N commands concurrently.\n\n");
		fprintf(stderr, "\tArguments: \n");
		fprintf(stderr, "\tCOMMAND: Full Path or relative path of linux command such as /bin/ls, /bin/ps,\n");
		fprintf(stderr, "\t/bin/date, /bin/who, /bin/uname etc\n");
		exit(0);
	}

	/*This allows the user to key in the arguments for the respective commands that he have entered earlier on. 
	  Do not put space after keying the arguments or NULL. NULL should be uppercase instead of lowercase. 
	*/
	for (counter = 1; counter < argc; counter++) {
		arguments[counter] = malloc(100);
		printf("Enter the argument for Command %s (enter NULL if no arguments): ", argv[counter]);
		fgets (arguments[counter], 100, stdin);
		arguments[counter][strlen(arguments[counter]) - 1] = '\0';      
	}


	/*Start time*/
	times(&tms_start);
	/*Creating child processes */
	create_child_processes(counter, argc, pid, argv, process, arguments);

	/*Wait for each child processes but not sequentially */
	wait_processes(child, status, argc, process, argv, tms_start, tms_end);

	/*All child processes finished executing*/
	printf("%s", "All done, bye!");
	return 0;
}



/* Takes a list of command line arguments each of which is the full path of a command
   Create N direct child processes according to the number of system command entered.
   Each child executing one of the N commands concurrently.
*/

void create_child_processes(int counter, int argc, pid_t pid, char *argv[], pid_t process[], char *arguments[]) {
	for (counter = 1; counter < argc; counter++) {
		pid = fork();

		/* Error creating child process */
		if (pid < 0) {
			perror("Forking Child Process Failed\n");
			exit(EXIT_FAILURE);
		}
		/* Executing the child process. If strcmp is not equal to 0, it means that it have argument specified for
	       the respective commands and thus will run with the argument else it will run without the argument.
		   If the execution of the child process failed, it will print out error message and exits the system.
		*/
		else if (pid == 0) {
			if (strcmp(arguments[counter], "NULL") != 0) {
				printf("%s",arguments[counter]);
				char *args[] = {argv[counter], arguments[counter], NULL};
				execvp(args[0], args);
				fprintf(stderr, "Command %s has not completed successfully\n\n", argv[counter]);
				exit(EXIT_FAILURE);

			} else {
				char *args[] = {argv[counter], NULL};
				execvp(args[0], args);
				fprintf(stderr, "Command %s has not completed successfully\n\n", argv[counter]);
				exit(EXIT_FAILURE);
			}
		}
		/*  Tracks the process that belonged to each child PID by storing it in an array.
			Element 0 is not in used in this situation
		*/
		else {
			process[counter] = pid;
		}

	}

}


/* This function will verify valid child processes by comparing with the array stored earlier on, to ensure that rogue processes will not be
   executing instead. If there is mismatching process number, it will return -1.
*/

int verify_child_processes(int pid, int number_of_processes, pid_t pids[]) {
	for (int counter = 1; counter < number_of_processes; counter++) {
		if (pids[counter] == pid) {
			return counter;
		}
	}
	return -1;
}

/* To make it concurrent, we cannot wait inside the loop as the child processes would then execute sequentially.
   waitpid(0) means that it will be waiting for any child process whose process group ID is equal to that of the calling process.
   WIFEXITED(status): child exited normally
   WEXITSTATUS(status): return code when child exits
   WIFSIGNALED(status): child exited because a signal was not caught
*/
void wait_processes(int child, int status, int argc, pid_t process[], char *argv[], struct tms tms_start, struct tms tms_end) {
	while ((child = waitpid(0, &status, 0)) > 0) {
		/* End time of each child processes*/
		times(&tms_end);
		int id = verify_child_processes(child, argc, process);
		if (id < 0) {
			fprintf(stderr, "Unknown PID %d exited", child);
		} else if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) == 0) {
				clock_t real = tms_end.tms_cstime - tms_start.tms_stime;
				float running_time = real / (double)sysconf(_SC_CLK_TCK);
				printf("Command %s has completed successfully at time %f\n\n", argv[id], running_time);
			}
		} else if (WIFSIGNALED(status)) {
			printf("Process %s (PID %d) was signalled %d\n", argv[id], child, WTERMSIG(status));
		} else {
			printf("Process %s (PID %d) died\n", argv[id], child);
		}

	}
}