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
void create_file(int fd1, int fd2);
int check_permission(struct stat file_info, char *cmd, char* msg, char* file_path, int mode);


int io_redirect_fd = -1;

#define READ 0
#define WRITE 1


int main(){
    // Create shell
    shell_loop();
    exit(EXIT_SUCCESS);
}

void shell_loop(){
    char *cmd;
    char **args;
    int status;
    int io_redirect;
    pid_t pid;

    do{
        printf("BudgetShell$ ");
        cmd = shell_read_line();
        args = shell_split_line(cmd);
        status = shell_execute_line(args);

    } while (SHELL);
}

char *shell_read_line(){

    int position = 0;
    int std_in;
    char *buffer = malloc(sizeof(char) * BUF_SIZE);

    while (READING_STDIN){
        std_in = getchar();

        if (std_in == EOF || std_in == '\n'){
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

char **shell_split_line(char *cmd){
    int position = 0;
    char **args = malloc(sizeof(char*) * BUF_SIZE);
    char *arg;
    struct stat buf;

    arg = strtok(cmd, SHELL_TOK_REDIRECT);
    arg = strtok(NULL, SHELL_TOK_REDIRECT);
    while (arg != NULL){
        char *write = arg, *read = arg;
        do {
            if (*read != ' ')
                *write++ = *read;
        } while (*read++);
        stat(arg, &buf);
        if (check_permission(buf, "BudgetShell", "", arg, WRITE)){
            return NULL;
        }
        io_redirect_fd = open(arg, O_WRONLY | O_CREAT, 0666);
        arg = strtok(NULL, SHELL_TOK_REDIRECT);
    }


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

int shell_execute_line(char **args){
    pid_t pid;
    int status;
    int saved_stdout;
    if ( strcmp(args[CMD], CMD_EXIT) == 0){
        return cmd_exit();
    }
    if ((pid = fork()) == -1){
        perror("Fork Error");
        exit(EXIT_FAILURE);
    } else if (pid == 0){
        if (io_redirect_fd > 1){
            saved_stdout = dup(STDOUT_FILENO);
            dup2(io_redirect_fd, STDOUT_FILENO);
            close(io_redirect_fd);
            io_redirect_fd = 1;
        } 
        if ( strcmp(args[CMD], CMD_PWD) == 0){
            status = cmd_pwd();
        } else if ( strcmp(args[CMD], CMD_COPY) == 0){
            status = cmd_copy(args);
        }

        //Restore STDOUT
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        exit(status);
    }
    pid = waitpid(pid, &status, 0);
    return status;
}

int cmd_pwd(){
    char current_directory[BUF_SIZE];
    getcwd(current_directory, BUF_SIZE);
    printf("%s\n", current_directory);
    return 0;
}

int cmd_copy(char **args){
    int num_arg = 0, source_fd, dest_fd;
    ssize_t nread;
    char *destination;
    char buf[BUF_SIZE];
    struct stat stat_source_buf, stat_dest_buf;

    do{
        if (args[num_arg + 1] == NULL){
            int is_file_exist = stat(*(args + num_arg), &stat_dest_buf);

            if (num_arg + 1 == 1){
                 printf("cp: missing file operand\n");
                 return 1;
             } else if (num_arg + 1 == 2){
                 printf("cp: missing destination file operand after '%s'\n", *(args + num_arg));
                 return 1;
             } else if (num_arg + 1 > 3){
                 if (is_file_exist < 0){
                     printf("cp: target '%s' is not a directory\n", *(args + num_arg));
                     return 1;
                 } else if (!S_ISDIR(stat_dest_buf.st_mode)){
                     printf("cp: target '%s' is not a directory\n", *(args + num_arg));
                     return 1;
                 }
             }
             destination = *(args + num_arg);
        }
        num_arg++;
    } while (*(args + num_arg) != NULL);

    for (int i = 1; i < num_arg - 1; i++){
        if (stat(*(args + i), &stat_source_buf) < 0){
            printf("cp: cannot stat '%s': No such file or directory\n", args[i]);
            continue;
        }
        mode_t mode = stat_source_buf.st_mode;
        source_fd = open(*(args + i), O_RDONLY);

        if (check_permission(stat_source_buf, CMD_COPY," cannot open", args[i], READ)){
            return 1;
        }

        if (S_ISDIR(mode)){
            printf("cp: omitting directory '%s'\n", args[i]);
            continue;
        }
        if (S_ISREG(mode) && !S_ISDIR(stat_dest_buf.st_mode)){
            if (check_permission(stat_dest_buf, CMD_COPY, " cannot create regular file", args[num_arg - 1], WRITE)){
                return 1;
            }

            dest_fd = open(args[num_arg - 1], O_WRONLY | O_CREAT, mode);
            create_file(source_fd, dest_fd);
        } else {
            char file_path[BUF_SIZE];
            snprintf(file_path, sizeof(file_path), "%s/%s", args[num_arg - 1], basename(args[i]));

            if (check_permission(stat_dest_buf, CMD_COPY ," cannot stat", file_path, WRITE)){
                return 1;
            }

            dest_fd = open(file_path, O_WRONLY | O_CREAT, mode);
            create_file(source_fd, dest_fd); 
        }
    }
}

#define USR_PER 0
#define GRP_PER 1
#define OTH_PER 2

int check_permission(struct stat file_info, char *cmd, char* msg, char *file_path, int mode){
    mode_t permission[2][3] = {{S_IRUSR, S_IRGRP, S_IROTH},{S_IWUSR, S_IWGRP, S_IWOTH}};
    char msg2[2][20] = {" for reading", ""};  

    if ( ((file_info.st_mode & permission[mode][USR_PER]) && getuid() == file_info.st_uid ||
          (getuid() != file_info.st_uid && getgid() == file_info.st_gid && (file_info.st_mode & permission[mode][GRP_PER])) ||
          (getuid() != file_info.st_uid && getgid() != file_info.st_gid && (file_info.st_mode & permission[mode][OTH_PER])))
                == 0){
        printf("%s:%s '%s'%s: Permission denied\n", cmd, msg, file_path, msg2[mode]);
        return 1;
    }
    return 0;
}


void create_file(int fd1, int fd2){
    char buf[BUF_SIZE];
    ssize_t nread;

    while((nread = read(fd1, buf, BUF_SIZE)) > 0){
        write(fd2, buf, nread);
    }
}

int cmd_exit(){
    exit(EXIT_SUCCESS);
}
