#ifndef HYPERGRAPH_H
#define HYPERGRAPH_H

# include "GHash.h"
# include "StronglyConnectedComponents.h"
# define DOUBLE_EDGE 2
# define EDGE_INSERTED 1
# define NEIGHTBOR_TABLE_START_SIZE 1

typedef struct HyperGraphNode{
    StrongComponent* nodeId;
    HashTable hashTable;
    int* neightborTable;
    int tableSize;
    int fillFactor;
}HyperGraphNode; 

typedef struct HyperGraph{
    struct HyperGraphNode* hNodeTable;
    int size;
    int* rootNodes;
    int rootNodesCount;
}HyperGraph;

void test(HyperGraph* hyperGraph,SCC* scc);
bool checkDublicateEdge(HyperGraph* hyperGraph ,StrongComponent* compS, StrongComponent* compF);
int insertEdge(HyperGraph* hyperGraph ,StrongComponent* compS, StrongComponent* compF );
HyperGraph* initHyperGraph(HyperGraph* hyperGraph);
HyperGraph* createHyperGraph(DoubleGraph* doubleGraph,SCC* scc);
void printHyperGraph(HyperGraph* hyperGraph);
void printRootNodes(HyperGraph* hyperGraph);
int destroyHyperGraph(HyperGraph* hyperGraph);
int destroyHyperHashTables(HyperGraph* hyperGraph);
HyperGraphNode* checkResizeNeightborTable(HyperGraphNode* hyperGraphNode);
#endif /* HYPERGRAPH_H */

