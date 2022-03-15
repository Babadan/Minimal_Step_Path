#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

#include <pthread.h>
#include "GraphQA.h"
#include "GQueue.h"
#include "Grail.h"

#define DEFAULT_TREAD_COUNT 16

typedef struct JobScheduler{
    int execution_threads;
    GQueue* queue; 
    pthread_t* tids; 
    
    pthread_mutex_t queueMutex; // queue    
    pthread_mutex_t batchMutex; // counter
    pthread_mutex_t queriesUsedUpdateIndexMutex; // counter
    
    
    pthread_mutex_t parkedThreadsMutex; // counter
    
    int batchfinished;
    
    int parkedThreads;
    
    pthread_cond_t cond_batch_finished;    
    pthread_cond_t cond_parkedMutex;
} JobScheduler;


typedef struct ThreadArgs{
    DoubleGraph* doubleGraph;
    JobScheduler* jobScheduler;
    int** results;
    char typeOfGraph;
    FILE* resultFile;
}ThreadArgs;

JobScheduler* initialize_scheduler( int execution_threads,ThreadArgs* threadArgs) ;

void submit_job( JobScheduler* sch, Job* j);

void execute_all_jobs( JobScheduler* sch);

void wait_all_tasks_finish( JobScheduler* sch);

int destroy_scheduler( JobScheduler* sch);



#endif /* JOBSCHEDULER_H */

