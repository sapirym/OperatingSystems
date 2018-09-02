/**
 * Sapir Yamin 316251818
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFF_SIZE 1
#define SPACE ' '
#define NEWLINE '\n'
#define TAB '\t'
#define FILE_ENDED 0
#define NUM_OF_ARGS 3
#define EQUAL 3
#define SIMILAR 2
#define NON 1
#define ERR_SIGN -1
#define ERR_EXIT 4
void closeReaders(int * file1, int * file2);
int checkSameEqual(char **argv);
int checkEqual(char *firstFile, char *secondFile2);
int checkSimilar(char *firstFile, char *secondFile2);
void lower(char * c1, char * c2);
int checkIfSpace(char buffer);
void errorHandler(int* file1, int* file2);

/**
 * main function
 * @param argc - number of arguments
 * @param argv - array of args
 * @return if the file are equal, same or not.
 */
int main(int argc, char **argv) {
    if (argc != NUM_OF_ARGS) {
        errorHandler(FILE_ENDED,FILE_ENDED);
        }
    return checkSameEqual(argv);
}

/**
 * handle error when it occurs
 * @param file1 the first file
 * @param file2 the second file
 */
void errorHandler(int* file1, int* file2){
    if(*file1!=FILE_ENDED)
        close(*file1);
    if (*file2 !=FILE_ENDED)
        close(*file2);
    write(2, "Error in system call\n", 21);
    exit(ERR_EXIT);
}

/**
 * function that close all readers.
 * @param file1 - the first file
 * @param file2 - the second file
 */
void closeReaders(int * file1, int * file2){
    close(*file1);
    close(*file2);
}

/**
 * check if there is space or new line.
 * @param buffer - the char buffer
 * @return 1 - if there is. 0- if not.
 */
int checkIfSpace(char buffer){
    if (buffer == SPACE || buffer == NEWLINE
            || buffer==TAB)
        return 1;
    return 0;
}

/**
 * convert the char to lower case
 * @param c1 - the first char
 * @param c2 - the second char
 */
void  lower(char * c1, char *c2 ) {
    if (*c1 <= 90 && *c1>= 65 ) {
        *c1 += 32;
    }
    if (*c2 <= 90 && *c2>= 65 ) {
        *c2 += 32;
    }
}

/**
 * call the functions that check if the file is same or equal.
 * @param argv - the array of args
 * @return the corrent number.
 */
int checkSameEqual(char **argv){
    if (checkEqual(argv[1], argv[2])) {
        return EQUAL;
    } else if (checkSimilar(argv[1], argv[2])) {
        return SIMILAR;
    } else {
        return NON;
    }
}
/**
 * check if the files are equal.
 * @param firstFile - the first file
 * @param secondFile - the second file
 * @return 1 - if equal, 0- if not.
 */
int checkEqual(char *firstFile, char *secondFile) {
    int result1, result2, first, second,  resultFlag = 1;
    char buffer1[BUFF_SIZE], buffer2[BUFF_SIZE];
    first = open(firstFile, O_RDONLY);
    if (first < 0) {
        errorHandler(FILE_ENDED,FILE_ENDED);
    }
    second = open(secondFile, O_RDONLY);
    if (second < 0) {
        errorHandler(&first,FILE_ENDED);
    }
    while (1) {
        result1 = read(first, buffer1, sizeof(char));
        result2 = read(second, buffer2, sizeof(char));
        if (result1 == ERR_SIGN || result2 == ERR_SIGN) {
            errorHandler(&first,&second);
        } else if (result1 == FILE_ENDED && result2 == FILE_ENDED) {
            break;
        } else if (*buffer1!=*buffer2){
            closeReaders(&first, &second);
            resultFlag= 0;
            break;
        } else if ((result1 == FILE_ENDED && result2 != FILE_ENDED)
                   || (result1 != FILE_ENDED && result2 == FILE_ENDED)) {
            closeReaders(&first, &second);
            resultFlag=0;
            break;
        }
    }
    closeReaders(&first, &second);
    return resultFlag;
}

/**
 * check if the files are similar to each other
 * @param firstFile - the first file
 * @param secondFile - the second file
 * @return 1 - if similar, 0 - if not.
 */

int checkSimilar(char *firstFile, char *secondFile) {
    int first, second, result1=1, result2=1, finalResult = 1;
    char buffer1[BUFF_SIZE],buffer2[BUFF_SIZE];
    first= open(firstFile, O_RDONLY);
    if (first < 0) {
        errorHandler(FILE_ENDED,FILE_ENDED);
    }
    second = open(secondFile, O_RDONLY);
    if (second < 0) {
        errorHandler(&first,FILE_ENDED);
    }
    while (result1!=FILE_ENDED&&result2!=FILE_ENDED) {
        result1 = read(first, buffer1, sizeof(char));
        result2 = read(second, buffer2, sizeof(char));
        if (result1 == ERR_SIGN|| result2 == ERR_SIGN) {
            errorHandler(&first, &second);
        }else if (*buffer1 != *buffer2) {
            lower(buffer1, buffer2);
            if (*buffer1 != *buffer2) {
                while (checkIfSpace(*buffer1)
                       && (result1 = read(first, buffer1, sizeof(char))) != FILE_ENDED) {
                    if (result1 == ERR_SIGN) {
                        errorHandler(&first, &second);
                    }
                    if (result2 == FILE_ENDED){
                        break;
                    }
                }
                while (checkIfSpace(*buffer2)
                       && (result2 = read(second, buffer2, sizeof(char))) != FILE_ENDED) {
                    if (result2 == ERR_SIGN) {
                        errorHandler(&first, &second);
                    }
                    if (result2 == FILE_ENDED){
                        break;
                    }

                }
                lower(buffer1, buffer2);
                if (*buffer1 != *buffer2) {
                    finalResult = 0;
                    break;
                }
            }
        } else if (result1 == FILE_ENDED && result2 != FILE_ENDED) {
            while (checkIfSpace(*buffer2)
                   && (result2 = read(second, buffer2, sizeof(char))) != FILE_ENDED) {
                if (result2 == ERR_SIGN) {
                    errorHandler(&first,&second);
                }
                if (result2 == FILE_ENDED){
                    break;
                }
            }
            if (!checkIfSpace(*buffer2)) {
                finalResult = 0;
                break;
            }else {
                break;
            }
        }else if (result1 != FILE_ENDED && result2 == FILE_ENDED) {
            while (checkIfSpace(*buffer1)
                   && (result1 = read(first, buffer1, sizeof(char))) != FILE_ENDED) {
                if (result1 == ERR_SIGN) {
                    errorHandler(&first, &second);
                }
                if (result1 == FILE_ENDED){
                    break;
                }
            }
            if (!checkIfSpace(*buffer1)) {
                finalResult = 0;
                break;
            }else {
                break;
            }
        }
    }
    closeReaders(&first,&second);
    return finalResult;
}

