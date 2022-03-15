#ifndef STRONGLYCONNECTEDCOMPONENTS_H
#define STRONGLYCONNECTEDCOMPONENTS_H
#include "GHash.h"
#include "Stack.h"
#include "Graph.h"

#define INVERTED_INDEX_START_SIZE 10
#define ALL_NEIGHBORS_EXPANDED -2

typedef struct StrongComponent{
    uint32_t component_id; 
    uint32_t included_nodes_count;
    uint32_t* included_node_ids;
}StrongComponent;

typedef struct SCC{
    StrongComponent* components;
    uint32_t components_count;
    StrongComponent** id_belongs_to_component;
    uint32_t id_belongs_size ;
}SCC;

typedef struct NodeInfo{
    uint32_t index;
    uint32_t lowlink;
    bool onStack;
}NodeInfo;

typedef struct DefinedNode{
    bool defined;// Stack pointer table;
    bool onStack;
    StackNode* stPtr;
}DefinedNode;

typedef struct DefinedMap{
   DefinedNode* map;
}DefinedMap;

typedef struct ComponentCursor{
    StrongComponent* component_ptr;
    int possition;
}ComponentCursor;



void printInvertedIndex(SCC* scc);
int initSCC(SCC* scc);
SCC* estimateStronglyConnectedComponents(DoubleGraph* doublegraph);
int estimateShortestPathStronglyConnectedComponents(SCC* stronglyConnectedComponents,DoubleGraph* doubleGraph ,uint32_t nodeS, uint32_t nodeF);
bool next_StronglyConnectedComponentID(SCC* scc , ComponentCursor* cursor);
int iterateStronglyConnectedComponentID(SCC* scc , ComponentCursor* cursor);
int findNodeStronglyConnectedComponentID(SCC* components, uint32_t node_Id);
bool sameComponent(SCC* scc,uint32_t nodeIdS, uint32_t nodeIdF );
int setIndexLowLink(StackNode* stackNode,uint32_t index,uint32_t lowLink);
long int getNextNeighbor(StackNode node,Graph graph);
int destroySronglyConnectedComponents(SCC* scc);
SCC* createInvertedIndex(SCC* scc,uint32_t biggestNode);

uint32_t min(uint32_t a , uint32_t b);
uint32_t max(uint32_t a , uint32_t b);

#endif /* STRONGLYCONNECTEDCOMPONENTS_H */

