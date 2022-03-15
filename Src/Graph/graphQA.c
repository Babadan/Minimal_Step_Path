#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "DatasetReader.h"
#include "Graph.h"
#include "Buffer.h"
#include "Queue.h"
#include "GraphQA.h"
#include "NodeIndex.h"
#include "Grail.h"
#include <assert.h>
#include "StronglyConnectedComponents.h"
#include "WeaklyConectedComponents.h"
#include "JobScheduler.h"

#define BYTES_TO_READ 30
#define START_QUEUE_SIZE 10

Job* getJob(FILE* workfile){
        
    char* tempBuff = calloc(sizeof(char),BYTES_TO_READ);
    char s[] = "\t\n ";
    char* token;
    char* buff = tempBuff;
     
    
    
    
    buff=fgets(buff,30,workfile);
    
    if(buff==NULL){
        free(tempBuff);
        return NULL;
    }
    Job* job = calloc(1,sizeof(Job));
    
    token= strtok(buff, s);
    job->type=token[0];
    while( token != NULL ){
       
         token= strtok(NULL, s);
         if(token == NULL)
             break;
         job->nodeS = atoi(token);
         
         token= strtok(NULL, s);
         if(token == NULL)
             break;
         job->nodeF = atoi(token);
        
    }


    free(buff);
    
    job->versioning = -1;
    
    return job;
}


int executeWorkLoadSeiriallyNoIndex(FILE* workFile,FILE*resultFile,DoubleGraph doubleGraph){
    
    Job* job = NULL;
    int* results = malloc(sizeof(int)*JOB_TABLE_START_SIZE);
    uint32_t size = JOB_TABLE_START_SIZE;
    uint32_t count = 0;
    uint32_t i;
    uint32_t  difference;
    
    int counter = 1;
    
    DoubleGraph* duoGraph = malloc(1*sizeof(DoubleGraph));
    
    for(i=0; i<size; i++)
        results[i]=-2;
    
    
    duoGraph->graphIngoing=doubleGraph.graphIngoing;
    duoGraph->graphOutgoing=doubleGraph.graphOutgoing;
    
    getJob(workFile);
    job = getJob(workFile);
    
    int line = 1;
    
    while(job!=NULL){
        
        if(job->type=='Q'){
            results[count]= shortestLengthBBFS(job,duoGraph);
            //results[count]= estimateShortestPathStronglyConnectedComponents(NULL,duoGraph,job->nodeS,job->nodeF);
            count++;
            line++;
            if(count == size){
                size = (count + 1 +count*20/100);
                results = realloc(results,sizeof(int)*size);
                difference = size-count;
                
                for(i=0; i<difference; i++){   
                    results[count+i]=-2;
                }
            }        
        }
        else if(job->type=='A'){
            addEdge(job,duoGraph);
        }
        else if(job->type=='F'){
            
            for(i=0; i<size; i++){
                if(results[i]==-2)
                    break;
                
                printf("%d\n",results[i]);
                fprintf(resultFile,"%d\n",results[i]);
            }
            
            for(i=0; i<size; i++){
                results[i]=-2;
            }
            
            count=0;
        }
        
        counter++;
        free(job);
        job=getJob(workFile);
    }
    
    free(results);
    free(duoGraph);
    
    return 0;
}

int executeWorkLoadParallel(FILE* workFile,FILE*resultFile){
    
    char buffer[20];

    fgets(buffer,20,workFile);
    
    
    if(strcmp(buffer,"STATIC\n")==0)
        staticWorkLoad(workFile,resultFile);
    else if(strcmp(buffer,"DYNAMIC\n")==0)
        dynamicWorkLoad(workFile,resultFile);
    
    return 0;
    
}

int staticWorkLoad(FILE* workFile,FILE*resultFile){
    
    Job* job = NULL;
    uint32_t jobId = 0;
    
    int** results = malloc(1*sizeof(int*));
    
    int i;
    
    JobScheduler* jobScheduler;
    int execution_threads = DEFAULT_TREAD_COUNT;
    ThreadArgs* threadArgs = malloc(1*sizeof(ThreadArgs));  
            
    HyperGraph* hyperGraph;
    
    dG.scc = estimateStronglyConnectedComponents(&dG);
    hyperGraph = createHyperGraph(&dG ,dG.scc);
    destroyHyperHashTables(hyperGraph);
    dG.grail = buildGrailIndex(hyperGraph,dG.scc,DEFAULT_LABLE_COUNT);
    destroyHyperGraph(hyperGraph);

    threadArgs->resultFile = resultFile;
    threadArgs->results = results;
    threadArgs->typeOfGraph = 'S';
    jobScheduler = initialize_scheduler(execution_threads,threadArgs);

   
    job=getJob(workFile);

    while(job!=NULL){

        if(job->type=='Q'){

            job->poss = jobId;
            submit_job(jobScheduler,job);
            jobId++;
        }
        else if(job->type=='A'){
            free(job);
        }
        else if(job->type=='F'){
            free(job);
            
            results[0] = calloc(jobId,sizeof(int));
            
            execute_all_jobs(jobScheduler);
            wait_all_tasks_finish(jobScheduler);
            
            for(i=0; i<jobId; i++){
				printf("%d\n",results[0][i]);
                fprintf(resultFile,"%d\n",results[0][i]);
            }
            jobId = 0;
            free(results[0]);
        }
        job=getJob(workFile);  
    }
    
    destroyGrailIndex(dG.grail); 
    destroySronglyConnectedComponents(dG.scc);
    destroy_scheduler(jobScheduler);    
    free(threadArgs);
    free(results);
    
    return 0;
    
}

int dynamicWorkLoad(FILE* workFile,FILE*resultFile){
    Job* job = NULL;
    uint32_t jobId = 0;

    int** results = malloc(1*sizeof(int*));
    int queriesCount = 0;
    int i;
    
    JobScheduler* jobScheduler;
    int execution_threads = DEFAULT_TREAD_COUNT;
    ThreadArgs* threadArgs = malloc(1*sizeof(ThreadArgs));  
           
       
    int currentVersion = 1;
    char previousJob = 'Q';

    dG.cc = estimateConnectedComponets(dG);
    threadArgs->resultFile = resultFile;
    threadArgs->results = results;
    threadArgs->typeOfGraph = 'D';
    jobScheduler = initialize_scheduler(execution_threads,threadArgs);

    job=getJob(workFile);

    while(job!=NULL){

        job->versioning = currentVersion;

        if(job->type=='Q'){

            previousJob = 'Q';   

            job->poss = jobId;
            submit_job(jobScheduler,job);
            jobId++;
            
            queriesCount++;
            
        }
        else if(job->type=='A'){

            if(previousJob=='Q')
                currentVersion++;

            previousJob = 'A';
            job->versioning = currentVersion;
            addEdge(job,&dG);
            insertNewEdge(dG.cc,job);
            free(job);
        }

        else if(job->type=='F'){
            free(job);
            
            results[0] = calloc(jobId,sizeof(int));
          
            execute_all_jobs(jobScheduler);
            wait_all_tasks_finish(jobScheduler);
            
            if(estimateMetric(dG.cc->metricVal,dG.cc->updateIndex->timesUsedCounter,queriesCount)){   
                pthread_mutex_lock(&(jobScheduler->queriesUsedUpdateIndexMutex));
                    rebuildIndexes(dG.cc);
                    destroyUpdateIndex(dG.cc->updateIndex);
                    dG.cc->updateIndex = malloc(1*sizeof(UpdateIndex));
                    initUpdateIndex(dG.cc->updateIndex);
                    dG.cc->updateIndex->timesUsedCounter=0;
                pthread_mutex_unlock(&(jobScheduler->queriesUsedUpdateIndexMutex));
                queriesCount=0;
            }
            
            for(i=0; i<jobId; i++){
				printf("%d\n",results[0][i]);
                fprintf(resultFile,"%d\n",results[0][i]);
            }
            
            jobId = 0;
            free(results[0]);
        } 

        job=getJob(workFile);

    }
    
    
    destroyConnectedComponents(dG.cc);
    destroy_scheduler(jobScheduler);    
    free(threadArgs);
    free(results);
    return 0;
    
}



DoubleGraph* executeWorkLoadSerially(FILE* workFile,FILE*resultFile,DoubleGraph* doubleGraph){
    
    Job* job = NULL;
    int* results = malloc(sizeof(int)*JOB_TABLE_START_SIZE);
    
    int counter = 1;
    int line = 1;
    int queriesCount = 0;
    int weaklyComponent;
    
    
    uint32_t size = JOB_TABLE_START_SIZE;
    uint32_t count = 0;
    uint32_t i;
    uint32_t difference;
    
    bool* updateTableUsed = malloc(1*sizeof(bool));
    
    HyperGraph* hyperGraph;
    
    char buffer[20];
    char dynamicStatic ;
    
    
    
    for(i=0; i<size; i++)
        results[i]=-2;
    
    fgets(buffer,20,workFile);
   
    if(strcmp(buffer,"STATIC\n")==0)
        dynamicStatic = STATIC;
    else if(strcmp(buffer,"DYNAMIC\n")==0)
        dynamicStatic = DYNAMIC;
    
    if(dynamicStatic == STATIC){
        doubleGraph->scc = estimateStronglyConnectedComponents(doubleGraph);
        hyperGraph = createHyperGraph(doubleGraph,doubleGraph->scc);
        destroyHyperHashTables(hyperGraph);
        doubleGraph->grail = buildGrailIndex(hyperGraph,doubleGraph->scc,DEFAULT_LABLE_COUNT);
        
    }
    else if(dynamicStatic == DYNAMIC){
        doubleGraph->cc = estimateConnectedComponets(*doubleGraph);
    }
    
    job = getJob(workFile);
    
    while(job!=NULL){
        
        if(job->type=='Q'){
            
            
            if(dynamicStatic == STATIC){
                results[count]= shortestLengthStaticBBFS(job,doubleGraph);
            }
            else if(dynamicStatic == DYNAMIC){
                weaklyComponent = nodesInSameComponent(doubleGraph->cc ,job,updateTableUsed);
                if(weaklyComponent == DIFFERENT_WEAKLY_COMPONENT)
                    results[count] = -1;
                else
                    results[count] = shortestLengthDynamicBBFS(job,doubleGraph,updateTableUsed);
                
                if(*updateTableUsed)
                    doubleGraph->cc->updateIndex->timesUsedCounter++;
                
                queriesCount++;
                
            }
            
            count++;
            line++;
            if(count == size){
                size = (count + 1 +count*20/100);
                results = realloc(results,sizeof(int)*size);
                difference = size-count;
                
                for(i=0; i<difference; i++){   
                    results[count+i]=-2;
                }
            }        
        }
        else if(job->type=='A'){
            
            if(dynamicStatic==DYNAMIC){
                addEdge(job,doubleGraph);
                insertNewEdge(doubleGraph->cc,job);
            }
            
        }
        else if(job->type=='F'){
            
            for(i=0; i<size; i++){
                if(results[i]==-2)
                    break;

                printf("%d\n",results[i]);
                fprintf(resultFile,"%d\n",results[i]);
            }
            
            for(i=0; i<size; i++){
                results[i]=-2;
            }
            if(dynamicStatic==DYNAMIC){
                if(estimateMetric(doubleGraph->cc->metricVal,doubleGraph->cc->updateIndex->timesUsedCounter,queriesCount)){
                    rebuildIndexes(doubleGraph->cc);
                    queriesCount=0;
                    doubleGraph->cc->updateIndex->timesUsedCounter = 0;  
                }
            }
            count=0;
        }
        
        counter++;
        free(job);
        job=getJob(workFile);
    }
    
    free(updateTableUsed);
    free(results);
    
    if(dynamicStatic==STATIC){
        destroyGrailIndex(doubleGraph->grail);
        destroyHyperGraph(hyperGraph);
        destroySronglyConnectedComponents(doubleGraph->scc);
    }
    if(dynamicStatic==DYNAMIC){
        destroyConnectedComponents(doubleGraph->cc);
    }
    
    return NULL;
}

int checkDoubleEdge(HashTable* hashTable,uint32_t neighbor){
    
    if(hashTable==NULL)
        return 0;
    
    if(getEntry(hashTable,neighbor)==NULL)
        return 0;
        
    return 1;
}

int addEdge(Job* job,DoubleGraph* doubleGraph){
    
    Graph* outGoingGraph = doubleGraph->graphOutgoing;
    NodeIndex* nodeIndexOut = doubleGraph->graphOutgoing->nodeIndex;
 
    long int buffoff;
    
    
    if(check_resize(nodeIndexOut,job->nodeS)==1){
        if(job->nodeF < doubleGraph->graphIngoing->biggestNode){
            buffoff = nodeIndexOut->indexTable[job->nodeF].bufferoffset[0];
            if(buffoff!=-1){
                if(checkDoubleEdge(outGoingGraph->nodeIndex->indexTable[job->nodeS].hashTable,job->nodeF)==1){
                    return 0;
                }
            }
        }
    }
    
    uint32_t* neighbor = malloc(4*sizeof(unsigned int));
    
    neighbor[0] = 1;
    neighbor[1] = job->nodeS;
    neighbor[2] = job->versioning;
    neighbor[3] = job->nodeF;
    
   
    insertNeighbors(doubleGraph->graphOutgoing,neighbor);
    
     
    neighbor[0] = 1;
    neighbor[1] = job->nodeF;
    neighbor[2] = job->versioning;
    neighbor[3] = job->nodeS;
    
    insertNeighbors(doubleGraph->graphIngoing,neighbor);
       
    free(neighbor);
    
    return 1;
    
    
}

int shortestLengthBBFS(Job* job, DoubleGraph* doubleGraph ){
    
    long int  i ;
    uint32_t distance=0;
    
    Queue* queueS = NULL ;
    Queue* queueF = NULL;
    
    list_node* neighborNodeS =NULL;
    list_node* neighborNodeF =NULL;
    
    long int  buffoffS;
    long int buffoffF;
     
    char* bufferPtrOut = doubleGraph->graphOutgoing->buffer->buffer;
    char* bufferPtrIn = doubleGraph->graphIngoing->buffer->buffer;
    
    NodeIndex* nodeIndexS = doubleGraph->graphOutgoing->nodeIndex;
    NodeIndex* nodeIndexF = doubleGraph->graphIngoing->nodeIndex ;
    
    if(nodeIndexS==NULL || nodeIndexF==NULL){
        perror("Node index not allocated something went wrong with graph loading\n");
    }
    
    if(job->nodeS>doubleGraph->graphOutgoing->biggestNode || job->nodeF>doubleGraph->graphIngoing->biggestNode){
        //printf("Start or Final node doesn't exist in the graph. \n");
        return -1;
    }
    
    buffoffS = getListHead(nodeIndexS ,job->nodeS);
    buffoffF = getListHead(nodeIndexF ,job->nodeF);
    if(buffoffS == -1 || buffoffF == -1){
        return -1;
    }
    neighborNodeS =(list_node*) (bufferPtrOut+buffoffS);
    neighborNodeF =(list_node*) (bufferPtrIn +buffoffF);
    
    
    if(job->nodeS == job->nodeF){
        return distance;
    }
    
    char* visitedTable = doubleGraph->visitedTable;
    if(doubleGraph->graphOutgoing->biggestNode>doubleGraph->graphIngoing->biggestNode)
        visitedTable = calloc(sizeof(char),doubleGraph->graphOutgoing->biggestNode+1);
    else
        visitedTable = calloc(sizeof(char),doubleGraph->graphIngoing->biggestNode+1);
    
    queueS = createQueue();
    queueF = createQueue();
    
     
    Node* popedNodeS=NULL;
    Node* popedNodeF=NULL;
    
    int cCurrentLayerS=1;
    int cNextLayerS=0;
    int cCurrentLayerF=1;
    int cNextLayerF=0;

    
    insertAtQueue(queueS, job->nodeS);
    insertAtQueue(queueF, job->nodeF);
    
     visitedTable[job->nodeS] = S_VISITED ;
     visitedTable[job->nodeF] = F_VISITED ;
    
    while(!isEmpty(queueS) && !isEmpty(queueF)){
    //while(!isEmpty(queueS)){  
    //--------------------------------Step 1---------------------------------    
        distance++;
        while(cCurrentLayerS!=0){
            popedNodeS = takeNode(queueS);
            cCurrentLayerS--;

            buffoffS = getListHead(nodeIndexS ,popedNodeS->node);
            if(buffoffS==-1){
                neighborNodeS = NULL;
            }
            else{
                neighborNodeS =(list_node*)(bufferPtrOut + buffoffS);
            }
            assert(nodeList_error=GRAPHNODE_NOT_FOUND); // Case the node doesn't exist .
            
            while(neighborNodeS!=NULL){
                for(i=0; i<neighborNodeS->fillFactor; i++){
                    uint32_t neighborId = neighborNodeS->neighbor[i];
                    //printf("%u, ",neighborNodeS->neighbor[i]);
                    if(visitedTable[neighborId] == F_VISITED){
                        free(visitedTable);
                        free(popedNodeS);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                       // printf("%u, ",distance);
                        return distance;
                    }
                    if(visitedTable[neighborId] == NOT_VISITED){
                        insertAtQueue(queueS,neighborId);
                        visitedTable[neighborId] = S_VISITED;
                        cNextLayerS++;
                    }
                }
                neighborNodeS =(list_node*) neighborNodeS->nextListNode;
            }
            //printf("\n");
            free(popedNodeS);
        }
        
        cCurrentLayerS = cNextLayerS;
        cNextLayerS = 0;
        
        distance++;
        while(cCurrentLayerF!=0){
            popedNodeF = takeNode(queueF);
            cCurrentLayerF--;

            buffoffF = getListHead(nodeIndexF ,popedNodeF->node);
            if(buffoffF==-1){
                neighborNodeF =NULL;
            }
            else{
                neighborNodeF =(list_node*)(bufferPtrIn + buffoffF);
            }

            nodeList_error=GRAPHNODE_NOT_FOUND;
            assert(nodeList_error=GRAPHNODE_NOT_FOUND); // Case the node doesn't exist .


            while(neighborNodeF!=NULL){

                for(i=0; i<neighborNodeF->fillFactor; i++){
                    uint32_t neighborId = neighborNodeF->neighbor[i];

                    if(visitedTable[neighborId] == S_VISITED){
                        free(visitedTable);
                        free(popedNodeF);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                        return distance;
                    }
                    if(visitedTable[neighborId] == NOT_VISITED){
                        insertAtQueue(queueF,neighborId);
                        visitedTable[neighborId] = F_VISITED;
                        cNextLayerF++;
                    }
                }
                neighborNodeF =(list_node*) neighborNodeF->nextListNode;   
            }
            free(popedNodeF);
        }
        
        cCurrentLayerF = cNextLayerF;
        cNextLayerF = 0;
    }    
        
    free(visitedTable);
    destroyQueue(queueS);
    destroyQueue(queueF);
                   
    return -1;
      
}

int shortestLengthDynamicBBFS(Job* job, DoubleGraph* doubleGraph,bool* updateIndexUsed){
    
    long int  i ;
    int weaklyComponent;
    uint32_t distance=0;
    
    Queue* queueS = NULL ;
    Queue* queueF = NULL;
    
    list_node* neighborNodeS =NULL;
    list_node* neighborNodeF =NULL;
    
    long int  buffoffS;
    long int buffoffF;
    
    
    
    char* bufferPtrOut = doubleGraph->graphOutgoing->buffer->buffer;
    char* bufferPtrIn = doubleGraph->graphIngoing->buffer->buffer;
    
    NodeIndex* nodeIndexS = doubleGraph->graphOutgoing->nodeIndex;
    NodeIndex* nodeIndexF = doubleGraph->graphIngoing->nodeIndex ;
    
    if(nodeIndexS==NULL || nodeIndexF==NULL){
        perror("Node index not allocated something went wrong with graph loading\n");
    }
    
    if(job->nodeS == job->nodeF){
        return distance;
    }
    
    if(job->nodeS>doubleGraph->graphOutgoing->biggestNode || job->nodeF>doubleGraph->graphIngoing->biggestNode){
        //printf("Start or Final node doesn't exist in the graph. \n");
        return -1;
    }
    
    weaklyComponent = nodesInSameComponent(doubleGraph->cc ,job,updateIndexUsed);
    if(weaklyComponent == DIFFERENT_WEAKLY_COMPONENT){
        return -1;
    }
    
    buffoffS = getListHead(nodeIndexS ,job->nodeS);
    buffoffF = getListHead(nodeIndexF ,job->nodeF);
    if(buffoffS == -1 || buffoffF == -1){
        return -1;
    }
    //////////////////////////
    
    
    
    neighborNodeS =(list_node*) (bufferPtrOut+buffoffS);
    neighborNodeF =(list_node*) (bufferPtrIn +buffoffF);
    
    
 
    char* visitedTable = doubleGraph->visitedTable;
    if(doubleGraph->graphOutgoing->biggestNode>doubleGraph->graphIngoing->biggestNode)
        visitedTable = calloc(sizeof(char),doubleGraph->graphOutgoing->biggestNode+1);
    else
        visitedTable = calloc(sizeof(char),doubleGraph->graphIngoing->biggestNode+1);
    
    queueS = createQueue();
    queueF = createQueue();
    
     
    Node* popedNodeS=NULL;
    Node* popedNodeF=NULL;
    
    int cCurrentLayerS=1;
    int cNextLayerS=0;
    int cCurrentLayerF=1;
    int cNextLayerF=0;

    
    insertAtQueue(queueS, job->nodeS);
    insertAtQueue(queueF, job->nodeF);
    
     visitedTable[job->nodeS] = S_VISITED ;
     visitedTable[job->nodeF] = F_VISITED ;
    
    while(!isEmpty(queueS) && !isEmpty(queueF)){
    //--------------------------------Step 1---------------------------------    
        distance++;
        while(cCurrentLayerS!=0){
            popedNodeS = takeNode(queueS);
            cCurrentLayerS--;

            buffoffS = getListHead(nodeIndexS ,popedNodeS->node);
            if(buffoffS==-1){
                neighborNodeS = NULL;
            }
            else{
                neighborNodeS =(list_node*)(bufferPtrOut + buffoffS);
            }
            
            while(neighborNodeS!=NULL){
                for(i=0; i<neighborNodeS->fillFactor; i++){
                    uint32_t neighborId = neighborNodeS->neighbor[i];
                    
                    if(neighborNodeS->edgeProperty[i]>job->versioning)
                        continue;
                    
                    if(visitedTable[neighborId] == F_VISITED){
                        free(visitedTable);
                        free(popedNodeS);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                      
                        return distance;
                    }
                    if(visitedTable[neighborId] == NOT_VISITED){
                        insertAtQueue(queueS,neighborId);
                        visitedTable[neighborId] = S_VISITED;
                        cNextLayerS++;
                    }
                }
                neighborNodeS =(list_node*) neighborNodeS->nextListNode;
            }
            
            free(popedNodeS);
        }
        cCurrentLayerS = cNextLayerS;
        cNextLayerS = 0;
        
        distance++;
        while(cCurrentLayerF!=0){
            popedNodeF = takeNode(queueF);
            cCurrentLayerF--;

            buffoffF = getListHead(nodeIndexF ,popedNodeF->node);
            if(buffoffF==-1){
                neighborNodeF =NULL;
            }
            else{
                neighborNodeF =(list_node*)(bufferPtrIn + buffoffF);
            }

            nodeList_error=GRAPHNODE_NOT_FOUND;
            
            while(neighborNodeF!=NULL){

                for(i=0; i<neighborNodeF->fillFactor; i++){
                    uint32_t neighborId = neighborNodeF->neighbor[i];
                    
                    if(neighborNodeF->edgeProperty[i]>job->versioning)
                        continue;
                    
                    if(visitedTable[neighborId] == S_VISITED){
                        free(visitedTable);
                        free(popedNodeF);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                        
                        return distance;
                    }
                    if(visitedTable[neighborId] == NOT_VISITED){
                        insertAtQueue(queueF,neighborId);
                        visitedTable[neighborId] = F_VISITED;
                        cNextLayerF++;
                    }
                }
                neighborNodeF =(list_node*) neighborNodeF->nextListNode;   
            }
            free(popedNodeF);
        }
        cCurrentLayerF = cNextLayerF;
        cNextLayerF = 0;
    }    
        
    free(visitedTable);
    destroyQueue(queueS);
    destroyQueue(queueF);
    
   
                   
    return -1;
      
}

int shortestLengthStaticBBFS(Job* job, DoubleGraph* doubleGraph ){
    
    long int  i,j ;
    uint32_t distance=0;
    
    Queue* queueS = NULL ;
    Queue* queueF = NULL;
    
    list_node* neighborNodeS =NULL;
    list_node* neighborNodeF =NULL;
    
    long int  buffoffS;
    long int buffoffF;
    
    
     if(job->nodeS == job->nodeF){
        return distance;
    }
    
    char* bufferPtrOut = doubleGraph->graphOutgoing->buffer->buffer;
    char* bufferPtrIn = doubleGraph->graphIngoing->buffer->buffer;
    
    NodeIndex* nodeIndexS = doubleGraph->graphOutgoing->nodeIndex;
    NodeIndex* nodeIndexF = doubleGraph->graphIngoing->nodeIndex ;
    
    if(nodeIndexS==NULL || nodeIndexF==NULL){
        perror("Node index not allocated something went wrong with graph loading\n");
    }
    
    if(job->nodeS > doubleGraph->graphOutgoing->biggestNode || job->nodeF > doubleGraph->graphIngoing->biggestNode){
        //printf("Start or Final node doesn't exist in the graph. \n");
        return -1;
    }
    
    
    if(sameComponent(doubleGraph->scc,job->nodeS,job->nodeF)){
        distance = estimateShortestPathStronglyConnectedComponents(doubleGraph->scc, doubleGraph ,job->nodeS,job->nodeF);
        return distance;
    }
    
    if(isReachableGrailIndex(doubleGraph->grail,doubleGraph->scc, job->nodeS,job->nodeF)==NO){
        return -1;
    }
    
    buffoffS = getListHead(nodeIndexS ,job->nodeS);
    buffoffF = getListHead(nodeIndexF ,job->nodeF);
    if(buffoffS == -1 || buffoffF == -1){
        return -1;
    }
    neighborNodeS =(list_node*) (bufferPtrOut+buffoffS);
    neighborNodeF =(list_node*) (bufferPtrIn +buffoffF);
    
    
    
    if(job->nodeS == job->nodeF){
        return distance;
    }
    
    char* visitedTable = doubleGraph->visitedTable;
    if(doubleGraph->graphOutgoing->biggestNode>doubleGraph->graphIngoing->biggestNode)
        visitedTable = calloc(sizeof(char),doubleGraph->graphOutgoing->biggestNode+1);
    else
        visitedTable = calloc(sizeof(char),doubleGraph->graphIngoing->biggestNode+1);
    
    queueS = createQueue();
    queueF = createQueue();
    
     
    Node* popedNodeS=NULL;
    Node* popedNodeF=NULL;
    
    int cCurrentLayerS=1;
    int cNextLayerS=0;
    int cCurrentLayerF=1;
    int cNextLayerF=0;

    
    insertAtQueue(queueS, job->nodeS);
    insertAtQueue(queueF, job->nodeF);
    
    visitedTable[job->nodeS] = S_VISITED ;
    visitedTable[job->nodeF] = F_VISITED ;
    
   
    
    while(!isEmpty(queueS) && !isEmpty(queueF)){ 
    //--------------------------------Step 1---------------------------------    
        distance++;
        while(cCurrentLayerS!=0){
            popedNodeS = takeNode(queueS);
            cCurrentLayerS--;

            buffoffS = getListHead(nodeIndexS ,popedNodeS->node);
            if(buffoffS==-1){
                neighborNodeS = NULL;
            }
            else{
                neighborNodeS =(list_node*)(bufferPtrOut + buffoffS);
            }
            assert(nodeList_error=GRAPHNODE_NOT_FOUND); // Case the node doesn't exist .
            
          
            while(neighborNodeS!=NULL){
                
                for(i=0; i<neighborNodeS->fillFactor; i++){
                    int reachabilityFlag = 0;
                    uint32_t neighborId = neighborNodeS->neighbor[i];
                    
                    if(visitedTable[neighborId] == F_VISITED){
                        free(visitedTable);
                        free(popedNodeS);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                        return distance;
                    }
                    if(visitedTable[neighborId] == NOT_VISITED){
                        Node* node = queueF->tail;
                        int reachable;
                        
                        for(j=0; j<cCurrentLayerF; j++){
                        //if(false){
                            if(sameComponent(doubleGraph->scc,neighborId,node->node)){
                                reachabilityFlag=1;
                                break;
                            }
                            else{
                                reachable = isReachableGrailIndex(doubleGraph->grail,doubleGraph->scc,neighborId,node->node);
                                if(reachable!=NO){
                                    reachabilityFlag=1;
                                    break;
                                }
                            }
                            node = node->next;
                        //}
                        }
                        
                        if(reachabilityFlag==1){
                            insertAtQueue(queueS,neighborId);
                            visitedTable[neighborId] = S_VISITED;
                            cNextLayerS++;
                        }
                    }
                }
                neighborNodeS =(list_node*) neighborNodeS->nextListNode;
            }
            
            free(popedNodeS);
        }
        
        cCurrentLayerS = cNextLayerS;
        cNextLayerS = 0;
        
        distance++;
        while(cCurrentLayerF!=0){
            popedNodeF = takeNode(queueF);
            cCurrentLayerF--;

            buffoffF = getListHead(nodeIndexF ,popedNodeF->node);
            if(buffoffF==-1){
                neighborNodeF =NULL;
            }
            else{
                neighborNodeF =(list_node*)(bufferPtrIn + buffoffF);
            }

            while(neighborNodeF!=NULL){
                
                
                for(i=0; i<neighborNodeF->fillFactor; i++){
                    int reachabilityFlag = 0;
                    uint32_t neighborId = neighborNodeF->neighbor[i];

                    if(visitedTable[neighborId] == S_VISITED){
                        free(visitedTable);
                        free(popedNodeF);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                        return distance;
                    }
                    if(visitedTable[neighborId] == NOT_VISITED){
                        
                        Node* node = queueS->tail;
                        int reachable;
                        
                        for(j=0; j<cCurrentLayerS; j++){
                        //if(false){
                            
                            if(sameComponent(doubleGraph->scc,node->node,neighborId)){
                                reachabilityFlag=1;
                                break;
                            }
                            else{
                                reachable = isReachableGrailIndex(doubleGraph->grail,doubleGraph->scc,node->node,neighborId);
                                if(reachable!=NO){
                                    reachabilityFlag=1;
                                    break;
                                }
                                node = node->next;
                            }
                        //}
                        }
                        
                        
                        if(reachabilityFlag==1){
                            insertAtQueue(queueF,neighborId);
                            visitedTable[neighborId] = F_VISITED;
                            cNextLayerF++;
                        }
                        
                    }
                }
                neighborNodeF =(list_node*) neighborNodeF->nextListNode;   
            }
            free(popedNodeF);
        }
        
        cCurrentLayerF = cNextLayerF;
        cNextLayerF = 0;
    }    
        
    free(visitedTable);
    destroyQueue(queueS);
    destroyQueue(queueF);
                   
    return -1;
      
}

