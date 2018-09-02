/**
 * sapir yamin 316251818
 */

#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "threadPool.h"

#define FAIL -1
#define SUCCEDED 1
void handleError();
void initializeThreadPool(ThreadPool *pool, int numOfThreads);
void checkResult(int result);
Task * initializeTask(void (*computeFunc)(void *), void *args);
void freeAll(ThreadPool* threadPool);
void* executeTask(void *args);
void executeTasks(void *args);
/**
 * handle in case of error
 */
void handleError(){
    int result ;
    if (result  = open("myFile.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) {
        exit(FAIL);
    }
    dup2(result, 1);
    write(2, "error in system call", 21);
    dup2(1,result);
    exit(FAIL);
}
/**
 * initialize threadpool
 * @param pool - the threadpool
 * @param numOfThreads - number of threads
 */
void initializeThreadPool(ThreadPool *pool, int numOfThreads ){
    pool->isStopped = 0;
    pool->numOfThreads = numOfThreads;
    pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * numOfThreads);
    pool->queue = osCreateQueue();
    pool->executeTasks = executeTasks;
    pool->status = RUN;
}
/**
 * check the result of function
 * @param result result of function
 */
void checkResult(int result){
    if (result) {
        handleError();
    }
}
/**
 * create threadPool
 * @param numOfThreads - number of threads
 * @return threadpool
 */
ThreadPool *tpCreate(int numOfThreads) {
    if(numOfThreads <= 0){
        handleError();
    }
    ThreadPool *threadPool = (ThreadPool*)malloc(sizeof(ThreadPool)*1);
    if (threadPool == NULL) {
        handleError();
    }
    initializeThreadPool(threadPool, numOfThreads);
    if ((*threadPool).threads== NULL) {
        handleError();
    }
    int result = pthread_mutex_init(&(*threadPool).mutexLock, NULL);
    checkResult(result);
    result = pthread_cond_init(&(*threadPool).threadCond, NULL);
    checkResult(result);
    for (int j = 0; j < numOfThreads; j++) {
        pthread_create(&(threadPool->threads[j]), NULL, executeTask, threadPool);
    }
    return threadPool;
}
/**
 * initialize task
 * @param computeFunc - compute function
 * @param args - the argss
 * @return task
 */
Task * initializeTask(void (*computeFunc)(void *), void *args){
    Task *task = (Task *)malloc(sizeof(Task));
    if (task == NULL) {
        handleError();
    }
    task->function = computeFunc;
    task->argument = args;
    return task;
}
/**
 * insert task
 * @param threadPool- the threadpool
 * @param computeFunc - the function
 * @param param - args
 * @return fail or success
 */
int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    if ((*threadPool).isStopped) {
        return FAIL;
    }
    Task * task = initializeTask(computeFunc, param);
    pthread_mutex_lock(&(*threadPool).mutexLock);
    osEnqueue((*threadPool).queue, task);
    int result = pthread_cond_signal(&(*threadPool).threadCond);
    checkResult(result);
    pthread_mutex_unlock(&threadPool->mutexLock);
    return SUCCEDED;
}
/**
 * execute one task
 * @param args parameters
 * @return function
 */
void *executeTask(void *args) {
    ThreadPool *threadPool = (ThreadPool*)args;
    (*threadPool).executeTasks(threadPool);
}
/**
 * execute all tasks
 * @param args parameters
 */
void executeTasks(void *args) {
    ThreadPool *threadPool;
    if(args!=NULL) {
        threadPool = (ThreadPool *) args;
        while (((*threadPool).status == DESTROY && osIsQueueEmpty((*threadPool).queue))
               || (*threadPool).status != FREE_STATE ||
               ((*threadPool).status == JOINTHREADS && (*threadPool).status != FREE_STATE)) {

            if (((*threadPool).status == JOINTHREADS || (*threadPool).status == RUN) &&
                osIsQueueEmpty((*threadPool).queue)) {
                pthread_mutex_lock(&(*threadPool).mutexLock);
                pthread_cond_wait(&((*threadPool).threadCond), &((*threadPool).mutexLock));
            } else if ((*threadPool).status == DESTROY && osIsQueueEmpty((*threadPool).queue)) {
                break;
            } else {
                pthread_mutex_lock(&(*threadPool).mutexLock);
            }
            if ((*threadPool).status != FREE_STATE && osIsQueueEmpty((*threadPool).queue)) {
                pthread_mutex_unlock(&(*threadPool).mutexLock);
            } else if (threadPool->status != FREE_STATE && !osIsQueueEmpty(threadPool->queue)) {
                Task *task = osDequeue((*threadPool).queue);
                pthread_mutex_unlock(&(*threadPool).mutexLock);
                (*task).function((*task).argument);
                free(task);
            }
            if ((*threadPool).status == JOINTHREADS && (*threadPool).status != FREE_STATE) {
                break;
            }
        }
    }
}
/**
 * destory all threads
 * @param threadPool -the threadpool
 * @param shouldWaitForTasks - int for status
 */
void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    pthread_mutex_lock(&(*threadPool).mutexLock);
    int result1 = pthread_cond_broadcast(&threadPool->threadCond);
    int result2 = pthread_mutex_unlock(&threadPool->mutexLock);
    if ( result1> 0 ||result2 > 0)
        handleError();
    if((*threadPool).isStopped)
        return;
    if (shouldWaitForTasks==0) {
        threadPool->status = JOINTHREADS;
    }
    else if (shouldWaitForTasks!=0) {
        threadPool->status = DESTROY;
    }
    threadPool->isStopped = 1;
    freeAll(threadPool);
}
/**
 * free all
 * @param threadPool- the threadpool
 */
void freeAll(ThreadPool* threadPool){
    for (int j = 0; j < (*threadPool).numOfThreads; j++)
        pthread_join((*threadPool).threads[j], NULL);
    threadPool->status = FREE_STATE;
    while (!osIsQueueEmpty((*threadPool).queue)) {
        Task *task = osDequeue((*threadPool).queue);
        free(task);
    }
    free(threadPool->threads);
    osDestroyQueue((*threadPool).queue);
    free(threadPool);
    pthread_mutex_destroy(&threadPool->mutexLock);
}