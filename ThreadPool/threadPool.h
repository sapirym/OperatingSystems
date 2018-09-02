/**
 * sapir yamin 316251818
 */
#ifndef __THREAD_POOL__
#define __THREAD_POOL__
#include "osqueue.h"
#include "pthread.h"

typedef struct Task{
    void (*function)(void *);
    void *argument;
} Task;

enum stateDestroy{RUN,DESTROY, JOINTHREADS,FREE_STATE};
typedef struct thread_pool
{
    int numOfThreads;
    int isStopped;
    enum stateDestroy status;
    pthread_t *threads;
    pthread_cond_t threadCond;
    OSQueue *queue;
    void (*executeTasks)(void *);
    pthread_mutex_t mutexLock;
}ThreadPool;

ThreadPool* tpCreate(int numOfThreads);
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

#endif