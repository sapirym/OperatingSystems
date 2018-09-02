/*
 * sapir yamin 316251818
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>
#include <wait.h>
#include <stdio.h>
#include <time.h>

#define MAX_LEN 200
struct Student {
    char d_name[MAX_LEN];
    char fileC[MAX_LEN];
    char grade[15];
    char result_msg[21];
};
#define BUFFER 1
#define NO_C_FILE_MSG "NO_C_FILE"
#define NO_C_FILE "0"
#define COMPILATION_ERROR_MSG "COMPILATION_ERROR"
#define COMPILATION_ERROR "0"
#define TIMEOUT_MSG "TIMEOUT"
#define TIMEOUT "0"
#define BAD_OUTPUT_MSG "BAD_OUTPUT"
#define BAD_OUTPUT "60"
#define SIMILAR_OUTPUT_MSG "SIMILAR_OUTPUT"
#define SIMILAR_OUTPUT "80"
#define GREAT_JOB_MSG "GREAT_JOB"
#define GREAT_JOB "100"
#define ERR_LEN 21
#define FAIL_SIGN -1
#define ERR_STD 2



void runCFile(struct Student *studentInfo, char *input, char *output);
void saveStudentInfo(struct Student *pStudent, char *grade, char *reason);
void saveStudentExe(struct dirent *item, struct Student *pStudent, int pos);
void makePath(char fullPath[MAX_LEN],char name[MAX_LEN], struct dirent *item);
int checkIfCFile(struct dirent *pDirent);
void checkIfFail(int result);
void startHandle(char **argv);
void findInDir(char *path, char *studentInfo);
void readConfFile(char **argv, int row, char param[MAX_LEN]);
void makeCompilation(struct Student *studentInfo, char *input, char *output);
void checkResultFile( char *result1, char *result2, struct Student *studentInfo);
int findTime(pid_t p);

/**
 * save info about the exe of student
 * @param item - the sirent
 * @param pStudent - the structure of the info
 * @param pos  - index
 */
void saveStudentExe(struct dirent *item, struct Student *pStudent, int pos){
    strcpy(pStudent[pos].d_name,item->d_name);
    strcpy(pStudent[pos].fileC,"");
}
/**
 * main function.
 * @param argc - the num of args
 * @param argv - the args
 * @return value
 */
int main(int argc, char **argv) {
    if (argc != 2){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    startHandle(argv);
    return 0;
}
/**
 * check if failed
 * @param result - the sign
 */
void checkIfFail(int result){
    if (result== FAIL_SIGN){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
}
/**
 * check the the path is correct
 * @param param - path
 */
void checkPath(char param[MAX_LEN]){
    if(strcmp(param, "") == 0){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(4);
    }
}
/**
 * check is dir is correct
 * @param dir - the dir
 * @param students - for free
 */
void checkDir(DIR *dir, struct Student *students){
    if (dir ==  NULL){
        free(students);
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
}
/**
 * handle the mission
 * @param argv - the args
 */
void startHandle(char **argv){
    int size = 5;
    struct Student *save;
    struct Student *studentsInfo = calloc(size,sizeof(struct Student));
    if (studentsInfo == NULL){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    char exeDir[MAX_LEN] ="";
    readConfFile(argv,1,exeDir );
    checkPath(exeDir);
    char args[MAX_LEN] ="";
    readConfFile(argv,2,args );
    checkPath(args);
    char resultPath[MAX_LEN] ="";
    readConfFile(argv,3,resultPath );
    checkPath(resultPath);
    DIR *dir = opendir(exeDir);
    checkDir(dir, studentsInfo);
    struct dirent *item;
    int pos = 0;
    char slash[2];
    slash[0] = '/';
    slash[1] = '\0';
    while ((item = readdir(dir)) != NULL) {
        char fullPath[MAX_LEN] ="";
        makePath(fullPath,exeDir, item);
        if (item->d_type == DT_DIR) {
            if (strcmp(item->d_name,".") != 0 && strcmp(item->d_name,"..") != 0){
                saveStudentExe(item,studentsInfo,pos);
                findInDir(fullPath, studentsInfo[pos].fileC);
                ++pos;
            }
        }
        if (pos == size-1){
            size += size;
            save =realloc( studentsInfo,sizeof(struct Student) *size);
            if (save ==NULL){
                free(studentsInfo);
                write(ERR_STD,"Error in system call\n",ERR_LEN);
                exit(FAIL_SIGN);
            }
            studentsInfo = save;
        }
    }
    char reset[MAX_LEN] = "";
    char dest[MAX_LEN] ="";
    int i = 0;
    while(i<pos){
        if (strcmp(studentsInfo[i].fileC,"") == 0){
            saveStudentInfo(&studentsInfo[i], NO_C_FILE, NO_C_FILE_MSG);
        } else{
            makeCompilation(&studentsInfo[i], args, resultPath);
        }
        i++;
    }
    int out = open("results.csv",O_CREAT| O_TRUNC| O_WRONLY,0666);
    if (out < 0){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    for (int j = 0; j < pos ;j++){
        int sizeDest = sizeof(dest);
        snprintf(dest, sizeDest,"%s,%s,%s\n", studentsInfo[j].d_name,
                 studentsInfo[j].grade ,studentsInfo[j].result_msg );
        checkIfFail(write(out,dest,strlen(dest)));
        memcpy(dest, reset, MAX_LEN);
    }
    if (closedir(dir) == FAIL_SIGN ||close(out)== FAIL_SIGN){
        free(studentsInfo);
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    free(studentsInfo);
}
/**
 * make path from two strings
 * @param fullPath - the result
 * @param name - the name of file
 * @param item - the item
 */
void makePath(char fullPath[MAX_LEN],char name[MAX_LEN], struct dirent *item){
    char slash[2];
    slash[0] = '/';
    slash[1] = '\0';
    strcat(fullPath, name);
    strcat(fullPath, slash);
    strcat(fullPath, item->d_name);
}
/**
 * read the configuration file
 * @param argv - args
 * @param row - the num of row
 * @param param - the string
 */
void readConfFile(char **argv, int row, char param[MAX_LEN]){
    char pathLine[MAX_LEN];
    int config = open(argv[1], O_RDONLY);
    if(config<0){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(4);
    }
    char buffer[BUFFER];
    int countRows = 0;
    while((read(config,buffer,sizeof(buffer)))>0 && countRows<row-1){
        if(*buffer=='\n'){
            countRows++;
        }
    }
    pathLine[0] = *buffer;
    int i =1;
    while((read(config,buffer,sizeof(buffer)))>0){
        if(*buffer == '\n'){
            break;
        } else{
            pathLine[i] = *buffer;
            i++;
        }
    }
    memcpy(param,pathLine,i);
}
/**
 * find in dir c files
 * @param name - the name
 * @param studentInfo - the struct info
 */
void findInDir(char *name, char *studentInfo){
    DIR *dir;
    if ((dir = opendir(name)) ==  NULL){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    struct dirent *item;
    while (( item= readdir(dir )) != NULL) {
        if (item->d_type !=DT_DIR) {
            if (!checkIfCFile(item)){ continue;}
            char save[MAX_LEN] ="";
            makePath(save, name, item);
            strcpy(studentInfo,save);
            if (closedir(dir) == FAIL_SIGN) {
                write(ERR_STD,"Error in system call\n",ERR_LEN);
                exit(FAIL_SIGN);
            }
            return;
        } else {
            char pathDir[MAX_LEN] = "";
            if ( strcmp(item->d_name, "..") == 0||strcmp(item->d_name, ".") == 0) {
                continue;
            }
            makePath(pathDir, name, item);
            findInDir(pathDir, studentInfo);
        }
    }
    if (closedir(dir) == FAIL_SIGN) {
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
}
/**
 * check if file is c type
 * @param pDirent - the struct
 * @return true is it is a c file.
 */
int checkIfCFile(struct dirent *pDirent) {
    const char *dotSign = strrchr(pDirent->d_name, '.');
    if (strcmp((dotSign),".c") == 0) {
        return 1;
    }
    if (!dotSign|| dotSign == pDirent->d_name) {
        return 0;
    }
    return 0;
}
/**
 * save the info about the student.
 * @param pStudent - the struct info
 * @param grade - the grade
 * @param reason - the reason
 */
void saveStudentInfo(struct Student *pStudent, char *grade, char *reason) {
    strcpy(pStudent->grade ,grade);
    strcpy(pStudent->result_msg, reason);
}
/**
 * make the compilation to c files.
 * @param studentInfo - the student info
 * @param input - the input file
 * @param output - output
 */
void makeCompilation(struct Student *studentInfo, char *input,
                     char *output) {
    char* args[3] = {"gcc", studentInfo->fileC,NULL};
    pid_t pid;
    pid = fork();
    if(pid<0){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    if (pid == 0){
        int result = execvp(args[0],args);
        if(result==-1) {
            write(ERR_STD,"Error in system call\n",ERR_LEN);
            exit(FAIL_SIGN);
        }
    } else {
        int status;
        if (waitpid(pid, &status, 0) == FAIL_SIGN){
            write(ERR_STD,"Error in system call\n",ERR_LEN);
            exit(FAIL_SIGN);
        }
        if (WEXITSTATUS(status) == 1){
            saveStudentInfo(studentInfo, COMPILATION_ERROR, COMPILATION_ERROR_MSG);
            return;
        } else {
            runCFile(studentInfo, input, output);
        }
    }
}
/**
 * run after compilation
 * @param studentInfo - the student info
 * @param input - the input
 * @param output - the output
 */
void runCFile(struct Student *studentInfo, char *input, char *output) {
    pid_t pid = fork();
    int time;
    char* args[2] = {"./a.out", NULL};
    if(pid<0){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    else if (pid == 0){
        int in = open(input,O_RDONLY);
        if (in < 0){
            write(ERR_STD,"Error in system call\n",ERR_LEN);
            exit(FAIL_SIGN);
        }
        int out = open("output.txt",  O_WRONLY | O_TRUNC |
                                             O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        if (out < 0) {
            write(ERR_STD,"Error in system call\n",ERR_LEN);
            exit(FAIL_SIGN);
        }
        checkIfFail(dup2(in,STDIN_FILENO) );
        checkIfFail(dup2(out,STDOUT_FILENO) );
        if (close(in) == FAIL_SIGN || close(out)){
            write(ERR_STD,"Error in system call\n",ERR_LEN);
            exit(FAIL_SIGN);
        }
        int result = execvp(args[0],args);
        checkIfFail(result);
    } else {
        time = findTime(pid);
        checkIfFail(unlink("a.out"));
        if (time == 0){
            saveStudentInfo(studentInfo, TIMEOUT, TIMEOUT_MSG);
            checkIfFail(unlink("output.txt") );
            return;
        }
        checkResultFile( "output.txt", output, studentInfo);
    }
}
/**
 * compare between the files.
 * @param first - the first file
 * @param sec - the sec file
 * @param studentInfo - the struct info
 */
void checkResultFile( char *first, char *sec, struct Student *studentInfo) {
    pid_t pid = fork();
    char* args[4]={"./comp.out",first,sec,NULL};
    if (pid<0){
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    }
    else if (pid==0){
        execvp(args[0],args);
        write(ERR_STD,"Error in system call\n",ERR_LEN);
        exit(FAIL_SIGN);
    } else {
        int status;
        checkIfFail(waitpid(pid,&status,0));
        int result = WEXITSTATUS(status);
        checkIfFail(unlink("output.txt") );
        switch (result){
            case 1:
                saveStudentInfo(studentInfo, BAD_OUTPUT, BAD_OUTPUT_MSG);
                break;
            case 2:
                saveStudentInfo(studentInfo, SIMILAR_OUTPUT, SIMILAR_OUTPUT_MSG);
                break;
            case 3:
                saveStudentInfo(studentInfo, GREAT_JOB, GREAT_JOB_MSG);
                break;
        }
    }
}
/**
 * check the time of the running for TIMEOUT error
 * @param p - the pid
 * @return if there is time error
 */
int findTime(pid_t p){
    int t = time(NULL)+5;
    while (time(NULL)<t){
        int status =waitpid(p,0,WNOHANG);

        if (status != 0 && status!=NULL){
            return 1;
        }
        checkIfFail(status);
    }
    return 0;
}
