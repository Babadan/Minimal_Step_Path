#ifndef GRAIL_H
#define GRAIL_H
#include "HyperGraph.h"

# define DEFAULT_LABLE_COUNT 3
# define NO_OUTGOING_EDGE -2 
# define ALL_NEIGHTBORS_EXPANDED -1
# define YES 2
# define MAYBE 1
# define NO 0


typedef struct Lable{
    
    int minRank;
    int rank;
    
}Lable;

typedef struct Tags{
    
    struct Lable* lables;
    
}Tags;

typedef struct GrailIndex{
  
    int size;
    int lableCount;
    struct Tags* tagsTable;
    
}GrailIndex;

void printGrail(GrailIndex grail,SCC* scc);
GrailIndex* buildGrailIndex(HyperGraph* hGraph, SCC* scc ,int lablesCount);
int compareLables(Lable lableS, Lable lableF);
int isReachableGrailIndex(GrailIndex* grail, SCC* scc, uint32_t nodeS,uint32_t nodeF);
GrailIndex* allocateGrail(int size,int lablesCount);
GrailIndex* intiGrailIndex(GrailIndex* grail,int size,int lablesCount);
int destroyGrailIndex(GrailIndex* grail);
int chooseStartNode(HyperGraph hyperGraph);
int chooseRandomNodeNeightborToExpand(HyperGraphNode* hyperGraphNode, int* offset);
int chooseConsecutiveNodeNeightborToExpand(HyperGraphNode* hyperGraphNode, int* offset);
void swapPlace(HyperGraphNode* hyperGraphNode, int offset, int randomPlace);


#endif /* GRAIL_H */

