#ifndef GRAPHCREATE_H
#define GRAPHCREATE_H
#include "Buffer.h"
#include "NodeIndex.h"

#define OUTGOING_EDGE 1;
#define INGOING_EDGE 1;

typedef struct Graph {
    Buffer* buffer;
    NodeIndex* nodeIndex;
    long int biggestNode;
    int inOut;
    
} Graph;

struct SCC;
struct CC;

typedef struct DoubleGraph{
    Graph* graphOutgoing;
    Graph* graphIngoing ;
    struct SCC* scc;
    struct CC* cc;
    struct GrailIndex* grail;
    char* visitedTable;
}DoubleGraph;

extern DoubleGraph dG;

DoubleGraph graphCreate(FILE* graphFile);
void printGraph(Graph* graph);
Graph* setBiggestNode(Graph* doubleGraph,uint32_t node);
int insertNeighbors(Graph* graph, uint32_t* neighborTable);
int destroyGraph(Graph* graph);
int connectIndexBuffer(Graph* graph,uint32_t nodeId);
uint32_t* dissectChunkIngoing(uint32_t** partGraph , long int size);
uint32_t* dissectChunkOutgoing(uint32_t** partGraph , long int size);
void printNeighbors(Graph* graph, int node);


#endif /* GRAPHCREATE_H */

