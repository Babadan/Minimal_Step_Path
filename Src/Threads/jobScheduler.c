#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Graph.h"
#include "JobScheduler.h"
#include "WeaklyConectedComponents.h"


void * threadWorker(void * argv) {
    unsigned long tid = (int) pthread_self();
    ThreadArgs * threadArgs = (ThreadArgs *) argv;
    //FILE* resultFile = threadArgs->resultFile;
    JobScheduler* js = threadArgs->jobScheduler;
    char typeOfGraph = threadArgs->typeOfGraph;
    int** res = threadArgs->results;
    DoubleGraph doubleGraph = dG;
    
    printf("worker started with tid: %lu \n", tid);

    while (1) {
        pthread_mutex_lock(&(js->batchMutex));
        while (js->batchfinished == 0) {
            
            pthread_mutex_lock(&(js->parkedThreadsMutex));
            js->parkedThreads++;
            pthread_mutex_unlock(&(js->parkedThreadsMutex));
            
            pthread_cond_broadcast(&(js->cond_parkedMutex));
            
            pthread_cond_wait(&(js->cond_batch_finished), &(js->batchMutex));
        }
        pthread_mutex_unlock(&(js->batchMutex));

        while (1) {
            
            pthread_mutex_lock(&(js->queueMutex));
            
            GQueueNode* gn = takefromGQueue(js->queue);
            
            if (gn == NULL) {
                pthread_mutex_lock(&(js->batchMutex));
                    
                    js->batchfinished = 0;
                
                pthread_mutex_unlock(&(js->batchMutex));
            }
            
            pthread_mutex_unlock(&(js->queueMutex));
            
            if (gn != NULL) {
                
                Job * job = (Job *) gn->value;
                int result;
                
                if (job->type == 'P') {
                    //pthread_mutex_lock(&(js->counterMutex));
                    
                        //js->number_of_jobs_completed++;
                    
                    //pthread_mutex_unlock(&(js->counterMutex));   
                    destroyGQueueNode(gn);
                    return 0;
                }
                
                if(typeOfGraph=='S'){
                    result = shortestLengthStaticBBFS(job,&doubleGraph);
                }
                else{
                    bool* updateIndexUsed = malloc(1*sizeof(bool));
                    result = shortestLengthDynamicBBFS(job,&doubleGraph,updateIndexUsed);
                    if(*updateIndexUsed){
                        pthread_mutex_lock(&(js->queriesUsedUpdateIndexMutex));
                            dG.cc->updateIndex->timesUsedCounter++;
                        pthread_mutex_unlock(&(js->queriesUsedUpdateIndexMutex));
                    }
                    free(updateIndexUsed);
                } 
                    
                
                
                //pthread_mutex_lock(&(js->counterMutex));
                
                
                res[0][job->poss] = result;
                destroyGQueueNode(gn);
//                while (js->number_of_jobs_completed != id - 1) {
//                    pthread_cond_wait(&(js->cond_job_finished), &(js->counterMutex));
//                }
                //js->number_of_jobs_completed++;
//                fprintf(resultFile,"%d\n",result);
//                
                //pthread_mutex_unlock(&(js->counterMutex));
                
                //pthread_cond_broadcast(&(js->cond_job_finished));
            } 
            else {
                break;
            }
        }
    }


    printf("worker exited\n");

    return NULL;
}

JobScheduler* initialize_scheduler(int execution_threads, ThreadArgs* threadArgs) {
    int i;
    
    
    JobScheduler* js = malloc(sizeof (JobScheduler));
    js->execution_threads = execution_threads;
    js->queue = createGQueue();
    js->tids = malloc(execution_threads * sizeof (pthread_t));
    js->batchfinished = 0;
    js->parkedThreads = 0;
    pthread_mutex_init(&(js->queueMutex), 0);
    pthread_mutex_init(&(js->batchMutex), 0);
    pthread_mutex_init(&(js->parkedThreadsMutex), 0);
    pthread_mutex_init(&(js->queriesUsedUpdateIndexMutex), 0);

    pthread_cond_init(&(js->cond_batch_finished), 0);
    pthread_cond_init(&(js->cond_parkedMutex), 0);
    
    threadArgs->jobScheduler =js;

    for (i = 0; i < js->execution_threads; i++) {
        pthread_create(&js->tids[i], NULL, threadWorker, threadArgs);
    }
    return js;
}

void submit_job(JobScheduler* js, Job* j) {
    GQueueNode * qn = createGQueueNode();
    qn->value = j;
    pthread_mutex_lock(&(js->queueMutex));
    insertAtGQueue(js->queue, qn);
    pthread_mutex_unlock(&(js->queueMutex));
}


void execute_all_jobs(JobScheduler* js) {
    
    pthread_mutex_lock(&(js->batchMutex));
    js->batchfinished = 1;
    js->parkedThreads =0 ;
    pthread_mutex_unlock(&(js->batchMutex));
    
    pthread_cond_broadcast(&js->cond_batch_finished);

    //printf("execution of all jobs started \n ");
}

void wait_all_tasks_finish(JobScheduler* js) {
    pthread_mutex_lock(&(js->parkedThreadsMutex));
        while (js->parkedThreads < js->execution_threads){
            pthread_cond_wait(&(js->cond_parkedMutex), &(js->parkedThreadsMutex));
        }
    pthread_mutex_unlock(&(js->parkedThreadsMutex));
}

int destroy_scheduler(JobScheduler* js) {
    int i;
    
    wait_all_tasks_finish(js);
    
    for (i = 0; i < js->execution_threads; i++) { // Poison batch 
        Job * job = malloc(sizeof (Job));
        job->type = 'P';
        job->nodeF = -1;
        job->nodeS = -1;
        job->poss = -1;
        submit_job(js, job);
    }

    execute_all_jobs(js);

    //printf("Waiting for all threads to exit ... \n");
    for (i = 0; i < js->execution_threads; i++) {
        pthread_join(js->tids[i], NULL);
    }

    pthread_cond_destroy(&js->cond_parkedMutex);
    pthread_cond_destroy(&js->cond_batch_finished);
    pthread_mutex_destroy(&(js->queueMutex));
    pthread_mutex_destroy(&(js->parkedThreadsMutex));
    free(js->tids);
    destroyGqueue(js->queue);
    free(js);
    return 0;
}


