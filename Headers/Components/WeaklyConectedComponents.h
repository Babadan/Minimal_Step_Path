
#ifndef CONECTEDCOMPONENTS_H
#define CONECTEDCOMPONENTS_H
#include "Queue.h"
#include "GHash.h"
#include "GQueue.h"
#include "GraphQA.h"

#define SAME_WEAKLY_COMPONENT 1
#define DIFFERENT_WEAKLY_COMPONENT 0
#define UPDATE_INDEX_START_SIZE 8
#define METRIC_VAL 3 



typedef struct UpdateIndex{
    HashTable* dHash;
    GQueue* valueQueue;
    int timesUsedCounter;
    HashTable* versioningHash; // This hash is double layer.
}UpdateIndex;

typedef struct WeakComponent{
    uint32_t component_id; 
    uint32_t included_nodes_count;
    uint32_t* included_node_ids;
}WeakComponent;

typedef struct CC{
    uint32_t* ccindex;
    UpdateIndex* updateIndex;
    double metricVal;
    uint32_t ccIndexSize;
    uint32_t componentsCount;
    WeakComponent** component;
}CC;

typedef struct Visited{
   struct Node** visitMap;
    
}Visited;


CC* estimateConnectedComponets(DoubleGraph graph);
UpdateIndex* initUpdateIndex(UpdateIndex* upIn);
int nodesInSameComponent(CC* components ,Job* job,bool* updateIndexUsed );
int insertNewEdge(CC* components, Job* job);
int destroyConnectedComponents(CC* components);
int destroyUpdateIndex(UpdateIndex* updateIndex);
int rebuildIndexes(CC* components);
int printCCIndex(CC* components);
bool estimateMetric(double metric,uint32_t queriesUsedUpdateIndex,uint32_t queries);
uint32_t findMinComp(uint32_t compIdS,uint32_t compIdE);
uint32_t findNodeConnectedComponentID(CC* components, uint32_t nodeId);
int destroyWeakComponent(WeakComponent* weakComponent);
char* createVersionHashKey(uint32_t compIdS,uint32_t compIdE);

#endif /* CONECTEDCOMPONENTS_H */

