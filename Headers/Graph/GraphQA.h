#ifndef GRAPHQA_H
#define GRAPHQA_H



#define DYNAMIC 'D'
#define STATIC 'S'
#define S_VISITED 'S'
#define F_VISITED 'F'
#define NOT_VISITED '\0'

#define JOB_TABLE_START_SIZE 20

#include "Graph.h"

typedef struct Job{
    char type;
    uint32_t nodeS ;
    uint32_t nodeF ;
    int versioning;
    int poss ; 
}Job;

int executeWorkLoadParallel(FILE* workFile,FILE* resultFile);

int staticWorkLoad(FILE* workFile,FILE*resultFile);
int dynamicWorkLoad(FILE* workFile,FILE*resultFile);

int shortestLengthStaticBBFS(Job* job, DoubleGraph* doubleGraph);
int shortestLengthDynamicBBFS(Job* job, DoubleGraph* doubleGraph,bool* updateIndexUsed);
int shortestLengthBBFS(Job* job,DoubleGraph* doubleGraph );

Job* getJob(FILE* workfile);
int addEdge(Job* job,DoubleGraph* doubleGraph);
int checkDoubleEdge(HashTable* hashTable,uint32_t neighbor);

DoubleGraph* executeWorkLoadSerially(FILE* workFile,FILE*resultFile,DoubleGraph* doubleGraph);
int executeWorkLoadSeiriallyNoIndex(FILE* workFile,FILE*resultFile,DoubleGraph doubleGraph);
//int checkDoubleEdge(struct list_node* listNode,uint32_t neighbor);
    


#endif /* GRAPHQA_H */

