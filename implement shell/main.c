/**
 * Sapir Yamin 31625818
 */

#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define JOBS_NUM 200
#define INPUTSIZE 550
#define FAIL_ALLOC "Error:The allocation failed.\n"

typedef struct job {
    pid_t pid;
    char **commandArgs;
};
int countJobs(struct job *jobs);
int showJobs(char **args, struct job *jobs, int jobsNum);
int exeJob(char **commandArgs, struct job *jobs);
int execute(struct job *jobs, char **command );
int checkSign(char **command);
int count(char **command);
int help();
void removeAll(struct job *jobs, int numJobs);
int cd(char **args, struct job *jobs);
char *getArgs();
char **splitLine(char *line);
void addJob(struct job *jobs, pid_t pid, char **command);
void deleteJob(struct job *jobs, pid_t pid);
/**
 * the main function.
 * @return zero.
 */
int main() {
    struct job *jobs = (struct job*)calloc(JOBS_NUM,sizeof(struct job));
    if (!jobs) {
        printf(FAIL_ALLOC);
        exit(0);
    }
    int status;
    char *commandLine;
    char **commandLineFree = (char **)calloc(JOBS_NUM,sizeof(char*));
    char **splitCommand;
    int i=0;
    do{
        printf("prompt>");
        commandLine = getArgs();
        commandLineFree[i] = commandLine;
        splitCommand = splitLine(commandLine);
        status = execute(jobs, splitCommand);
        free (splitCommand);
        i++;
    }while (status == 1);
    for(int j=0;commandLineFree[j]!=NULL;j++)
        free(commandLineFree[j]);
    free(commandLineFree);
    free(jobs);
    return 0;
}



void checkJobs(struct job *jobs) {
    int notRunningPid[JOBS_NUM];
    int status;
    for (int i=0;jobs[i].commandArgs!=NULL;i++) {
        if (waitpid(jobs[i].pid, &status, WNOHANG != 0)) {
            notRunningPid[i] = jobs[i].pid;
            deleteJob(jobs, notRunningPid[i]);
        }
    }

}

/**
 * get from the user the command.
 * @return the line of the command.
 */
char *getArgs() {
    char *command = (char *) calloc(INPUTSIZE, sizeof(char));
    if (!command) {
        printf(FAIL_ALLOC);
        exit(0);
    }
    int counter = 0, c;
    while (1) {
        c = getchar();
        if (c != '\n') {
            command[counter] = c;
            counter++;
        } else {
            command[counter] = '\0';
            return command;
        }
        size_t buffer_size = INPUTSIZE;
        if (counter >=buffer_size) {
            buffer_size+= INPUTSIZE;
            command = (char*)realloc(command, buffer_size * sizeof(char));
            if ( !command) {
                free(command);
                printf(FAIL_ALLOC);
                exit(0);
            }
        }
    }
}

/**
 * split line into words.
 */
char **splitLine(char *line) {
    char **strings = (char**)calloc(INPUTSIZE, sizeof(char *));
    if (!strings) {
        printf(FAIL_ALLOC);
        exit(0);
    }
    int pos = 0, buffer_size = INPUTSIZE;
    char *token = strtok(line, " ");
    while (token!= NULL) {
        strings[pos] = token;
        pos++;
        if (buffer_size<=pos) {
            buffer_size += INPUTSIZE;
            strings = (char **) realloc(strings, buffer_size * sizeof(char));
            if (!strings) {
                printf(FAIL_ALLOC);
                exit(0);
            }
        }
        token = strtok(NULL, " ");
    }
    return strings;
}

/**
 * run the command of the user.
 * @param: jobs -  all the jobs that run.
 * @param: command - the new command of the user.
 * return 1 if can go on - 0 if not.
 */
int execute(struct job *jobs, char **command ) {
    int jobsNum = countJobs(jobs);
    int result=0;
    if (command == NULL || command[0] == NULL) {
        result = 1;
    }else if (strcmp(command[0],"cd") == 0){
        result =  cd(command, jobs);
    } else if (strcmp(command[0],"jobs") == 0){
        result=  showJobs(command, jobs, jobsNum);
    } else if (strcmp(command[0],"help") == 0){
        result = help();
    }else if (strcmp(command[0],"exit") == 0){
        removeAll(jobs, jobsNum);
        result  = 0;
    } else {
        result  =  exeJob(command, jobs);
    }
    return  result;
}

/**
 * check if there is &.
 * @param command the args of the command.
 * @return 1 - if there is  &, 0 - else.
 */
int checkSign(char **command) {
    int i= count(command);
    if (!strcmp(command[i - 1], "&") )
        return 1;
    return 0;
}


/**
 * check the number of elements in array.
* @param command - the array
* @return number of ele
*/
int count(char **command) {
    int i=0;
    for (; command[i] != NULL; i++);
    return i;
}
/**
 * check the number of elements in array.
* @param command - the array
* @return number of ele
*/
int countJobs(struct job *jobs) {
    int i=0;
    for (; jobs[i].commandArgs != NULL; i++);
    return i;
}

/**
 * executes the command.
 * @param command array of command arguments.
 * @param jobs array of jobs.
 * @return if succeed - return 1, else - 0 .
 */
int exeJob(char **commandArgs, struct job *jobs) {
    int sign = checkSign(commandArgs);
    if (sign == 1) {
        int i=count(commandArgs);
        commandArgs[i -1] = '\0';
    }
    pid_t pid = fork();
    if (pid == 0) {
        execvp(commandArgs[0], commandArgs);
        deleteJob(jobs, pid);
        fprintf(stderr, "Error in system call\n");
    } else {
        if (pid > 0) {
            printf("%d\n", pid);
            addJob(jobs, pid, commandArgs);
        } else {
            perror("Failed with forking\n");
            return 0;
        }
        if (sign == 0 && pid != 0) {
            if(waitpid(pid, NULL, 0)==pid) {
                deleteJob(jobs, pid);
            }
        }
    }
    return 1;
}

/**
 * Change directory command.
 * @param input array of command arguments.
 * @param jobs array of jobs.
 * @return true if success, false if fail.
 */
int cd(char **args, struct job *jobs) {
    pid_t pid = getpid();
    printf("%d\n", pid);
    char * dir = args[1];
    if (dir != NULL) {
        if (chdir(dir) == -1) {
            printf("Error: %s - Can't find directory\n", dir);
        }
    } else {
        chdir(getenv("HOME"));
    }
    return 1;
}

/**
 * if the user chose the help command
 */
int help() {
    printf("help:\n"
                   "cd - move to other directory.\n"
                   "jobs - get all jobs that currently run.\n"
                   "exit - finish the program.\n");
    return 1;
}

/**
 * print the current jobs.
 * @param jobs - the array of jbs
 */
void printArr(struct job jobs) {
    int i = 0;
    if (jobs.pid != -1) {
        printf("%d\t\t", jobs.pid);
        while (jobs.commandArgs[i] != NULL) {
            printf("\t%s", jobs.commandArgs[i]);
            printf("\t\t");
            i++;
        }
        printf("\n");
    }
}



/**
 * add job to the array.
 * @param jobs - the array of the jobs.
 * @param pid - the pid of the job.
 * @param command - the command args array.
 */
void addJob(struct job *jobs, pid_t pid, char **command) {
    int size = 0, i= 0;
    while (i!=JOBS_NUM){
        if (jobs[i].pid == 0) {
            jobs[i].pid = pid;
            size = sizeof(command[0])/sizeof(command[0][0]);
            jobs[i].commandArgs = calloc(size, sizeof(char*));
            memcpy(jobs[i].commandArgs, command, size *sizeof(char*));
            break;
        }
        i++;
    }
}

/**
 * if the user chose the jobs command
 */
int showJobs(char **args, struct job *jobs, int jobsNum) {
    checkJobs(jobs);
    if (jobs == NULL){
        return 1;
    }
    int i=0;
    while (i!=JOBS_NUM){
        if (jobs[i].pid != 0 && jobs[i].pid!= -1){
            printArr(jobs[i]);
        }
        i++;
    }
    return 1;
}
/**
 * delete job from the list using pid.
 */
void deleteJob(struct job *jobs, pid_t pid) {
    for (int i = 0; i < JOBS_NUM; i++) {
        if (jobs[i].pid != -1 && jobs[i].pid == pid && jobs[i].commandArgs!=NULL) {
            jobs[i].pid =-1;
            free(jobs[i].commandArgs);
        }
    }
}

/**
 * delete all jobs
 */
void removeAll(struct job *jobs, int numJobs) {
    int i=0;
    while (i !=numJobs) {
        if (jobs[i].commandArgs != NULL && jobs[i].pid!=-1) {
            free(jobs[i].commandArgs);
        }
        i++;
    }
}