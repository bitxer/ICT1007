#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/wait.h>

#define BUF_SIZE 512
#define READING_STDIN 1
#define SHELL 1

void shell_loop();
char *shell_read_line();
char **shell_split_line(char *cmd);
int shell_execute_line(char **args);
int cmd_pwd();
int cmd_exit();
int cmd_copy(char **args);
void copy_file(int fd1, int fd2);
void close_files(int fd1, int fd2);

int io_redirect_fd = -1;

#define READ 0
#define WRITE 1


int main(){
    // Create shell
    shell_loop();
    exit(EXIT_SUCCESS);
}

/**
 *  Continous Accept Command until the Shell terminates
 */
void shell_loop(){
    char *cmd;
    char **args;
    int status;
    // The Shell will perform the following actions
    //  1.  Read user input
    //  2.  Split the user input into arguments
    //  3.  Execute the command by the user
    //  4.  Go to Step 1
    do{
        printf("BudgetShell$ ");
        cmd = shell_read_line();
        args = shell_split_line(cmd);
        if (args != NULL){
            status = shell_execute_line(args);
        }

    } while (SHELL);
}

/**
 *  Gets the input of user from STDIN
 * 
 *  Return:
 *      buffer  -   a string containing the command input by user
 */
char *shell_read_line(){

    //the current position of the buffer
    int position = 0;

    // temporary store for a character
    int std_in;
    char *buffer = malloc(sizeof(char) * BUF_SIZE);

    // Loops continous until EOF or ENTER key is received
    while (READING_STDIN){
        std_in = getchar();

        //Check if EOF or ENTER is recieved
        if (std_in == EOF || std_in == '\n'){
            //Set the last character to be a null character
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = std_in;
        }
        position++;
    }
}


#define SHELL_TOK_REDIRECT ">"
#define SHELL_TOK_ARGS " \t\r\n\a"

/**
 *  Splits the command given by the user into arguments
 *  and create files used for I/O redirection
 * 
 *  Arguments:
 *      cmd     -   the string to extract the arguments from
 * 
 *  Returns:
 *      Success
 *      args    -   an array of arguments to execute
 *      Fail
 *      NULL    -   If no permission is given to create/access the file, command will not be executed
 *      
 */

char **shell_split_line(char *cmd){
    int position = 0;
    char **args = malloc(sizeof(char*) * BUF_SIZE);
    char *arg;
    struct stat buf;

    //  Check for I/O Redirection
    arg = strtok(cmd, SHELL_TOK_REDIRECT);
    arg = strtok(NULL, SHELL_TOK_REDIRECT); // gets the string after '>'

    // Loops until no more redirection is found
    while (arg != NULL){
        char *write = arg, *read = arg;

        //Strips white space in characters
        do {
            if (*read != ' ')
                *write++ = *read;
        } while (*read++);

        //Gets fd for the io redirect
        io_redirect_fd = open(arg, O_WRONLY | O_CREAT, 0666);
        if (io_redirect_fd == -1){
            printf("BudgetShell: cannot create regular file '%s': Permission denied\n", arg);
            return NULL;
        }
        arg = strtok(NULL, SHELL_TOK_REDIRECT);
    }

    // Gets the command and arguemnts
    arg = strtok(cmd, SHELL_TOK_ARGS);

    while (arg != NULL){
            args[position] = arg;
            position++;

        arg = strtok(NULL, SHELL_TOK_ARGS);
    }
    args[position] = NULL;

    return args;
}

#define CMD 0
#define CMD_PWD "pwd"
#define CMD_EXIT "exit"
#define CMD_COPY "cp"

/**
 *  Execute command that was given by user by forking it
 * 
 *  Arguments:
 *      args    -   command to be executed
 * 
 *  Return:
 *      status  -   the status of the executed process
 */
int shell_execute_line(char **args){
    pid_t pid;
    int status;
    int saved_stdout;
    // Check if exit command is given to exit the shell
    if ( strcmp(args[CMD], CMD_EXIT) == 0){
        return cmd_exit();
    }

    // forks the process to run the command
    if ((pid = fork()) == -1){
        perror("Fork Error");   //If failed, close the process
        exit(EXIT_FAILURE);
    } else if (pid == 0){   //Enter here if the process is the child
        if (io_redirect_fd > 1){                    // Check if it is io redirection
            saved_stdout = dup(STDOUT_FILENO);      //Save a copy of stdout to restore after child execution
            dup2(io_redirect_fd, STDOUT_FILENO);    // dup io redirect fd so that stdout will be sent to file instead of command prompt
            close(io_redirect_fd);
            io_redirect_fd = 1;
        } 
        if ( strcmp(args[CMD], CMD_PWD) == 0){      //Checks if cmd is "pwd"
            status = cmd_pwd();
        } else if ( strcmp(args[CMD], CMD_COPY) == 0){  // Checks if cmd is "cp" 
            status = cmd_copy(args);
        } else {
            printf("Command '%s' not found.\n", args[CMD]);
        }

        //Restore STDOUT
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        exit(status);   //Command execution ends here
    }
    pid = waitpid(pid, &status, 0); // Waits for command to finish executing
    return status;
}

/**
 *  Prints the Current Working Directory
 */
int cmd_pwd(){
    char current_directory[BUF_SIZE];
    getcwd(current_directory, BUF_SIZE);
    printf("%s\n", current_directory);
    return 0;
}

/**
 *  Executes the copy command given by user
 * 
 *  Arguments:
 *      args    - contains the arguments that will be used with cp cmd
 */

int cmd_copy(char **args){
    int num_arg = 0, source_fd, dest_fd;
    ssize_t nread;
    char *destination;
    char buf[BUF_SIZE];
    struct stat stat_source_buf, stat_dest_buf;

    //  Check if cp command is supplied with valid arguments
    do{
        if (args[num_arg + 1] == NULL){
            // Check if destination file exist
            int is_file_exist = stat(*(args + num_arg), &stat_dest_buf);

            if (num_arg + 1 == 1){  //Check if command is supplied with 'cp' and no other arguments
                 printf("cp: missing file operand\n");
                 printf("cp [source] [destination]\ncp [source] ... [directory]");
                 return 1;
             } else if (num_arg + 1 == 2){  //Check if destination file/directory is given
                 printf("cp: missing destination file operand after '%s'\n", *(args + num_arg));
                 return 1;
             } else if (num_arg + 1 > 3){ //Check if the target exist and it is a directory
                 if (is_file_exist < 0){
                     printf("cp: target '%s' is not a directory\n", *(args + num_arg));
                     return 1;
                 } else if (!S_ISDIR(stat_dest_buf.st_mode)){
                     printf("cp: target '%s' is not a directory\n", *(args + num_arg));
                     return 1;
                 }
             }
             destination = *(args + num_arg); //Save the name of the destination
        }
        num_arg++;
    } while (*(args + num_arg) != NULL);

    // This loop will copy files from the 1st argument to num_arg-1 argument
    for (int i = 1; i < num_arg - 1; i++){
        // Checks if the source file exist
        if (stat(*(args + i), &stat_source_buf) < 0){
            printf("cp: cannot stat '%s': No such file or directory\n", args[i]);
            continue;
        }

        mode_t mode = stat_source_buf.st_mode;
        source_fd = open(*(args + i), O_RDONLY);

        if (source_fd == -1){   //Check if permission is given to access file
            printf("cp: cannot open '%s' for reading: Permission denied\n", args[i]);
            return 1;
        }

        if (stat_source_buf.st_ino == stat_dest_buf.st_ino){ // prevents copy if both the source and destination are the same file name
            printf("cp: '%s' and '%s' are the same file\n", *(args + i), destination);
            close(source_fd);
            continue;
        }

        if (S_ISDIR(mode)){ //copy directory to directory will be excluded
            printf("cp: omitting directory '%s'\n", args[i]);
            close(source_fd);
            continue;
        }
        if (S_ISREG(mode) && !S_ISDIR(stat_dest_buf.st_mode)){ //Copy from file to file

            dest_fd = open(args[num_arg - 1], O_WRONLY | O_CREAT, mode);
            if (dest_fd == -1){ // Check if permission is given to write
                printf("cp: cannot create regular file '%s': Permission denied\n", args[num_arg - 1]);
                close_files(source_fd, dest_fd);
                return 1;
            }
                copy_file(source_fd, dest_fd);
                close_files(source_fd, dest_fd);

        } else {// Copy from file to directory
            char file_path[BUF_SIZE];
            struct stat stat_dest;
            snprintf(file_path, sizeof(file_path), "%s/%s", args[num_arg - 1], basename(args[i]));

            dest_fd = open(file_path, O_WRONLY | O_CREAT, mode);

            if (dest_fd == -1){ //Check if permission is given to acess directory
                printf("cp: cannot stat '%s': No such file or directory\n", file_path);
                close_files(source_fd, dest_fd);
                return 1;
            }
            fstat(dest_fd, &stat_dest);
            if (stat_source_buf.st_ino == stat_dest.st_ino){ // Check for same file name
                printf("cp: '%s' and '%s' are the same file\n", *(args + i), file_path);
                close_files(source_fd, dest_fd);
                return 1;
            }

            // Start copy
            copy_file(source_fd, dest_fd); 
            close_files(source_fd, dest_fd);
        }
    }
}

/**
 *  Copy contents of fd1 to fd2
 * 
 *  Arguments:
 *      fd1     -   file to be copied
 *      fd2     -   file to write the contents to
 */
void copy_file(int fd1, int fd2){
    char buf[BUF_SIZE];
    ssize_t nread;

    while((nread = read(fd1, buf, BUF_SIZE)) > 0){
        write(fd2, buf, nread);
    }
}
/**
 *  Closes file decripters
 */
void close_files(int fd1, int fd2){
    close(fd1);
    close(fd2);
}

/**
 *  Ends the proccess and exits the Shell
 */
int cmd_exit(){
    exit(EXIT_SUCCESS);
}
