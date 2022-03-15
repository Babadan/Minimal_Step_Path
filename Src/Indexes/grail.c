#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Grail.h"
#include "GStack.h"
#include "Graph.h"
#include "GraphQA.h"
#include "CompareResults.h"
#include "WeaklyConectedComponents.h"
#include "StronglyConnectedComponents.h"
#include "HyperGraph.h"



void printGrail(GrailIndex grail,SCC* scc){
    int i,j ;
    
    for(i=1; i<grail.size; i++){
        printf("%d->",i);
        for(j=0; j<grail.lableCount; j++){
            if(grail.tagsTable[i].lables[j].minRank==-1){
                printf("[%d,%d] ",grail.tagsTable[i].lables[j].minRank,grail.tagsTable[i].lables[j].rank);
        
            }
        }
        printf("\n");
    }
}

GrailIndex* buildGrailIndex(HyperGraph* hGraph, SCC* scc ,int lablesCount){
    
    
    if(hGraph==NULL){
        printf("Graph not allocated.\n");
        return NULL;
    }
    
    if(scc==NULL){
        printf("SCC not allocated.\n");
        return NULL;
    }
   
    time_t t;
    
    srand((unsigned) time(&t));
    //Stack* stack = createStack();
    
    if(lablesCount==-1)
        lablesCount = DEFAULT_LABLE_COUNT;
    
    
    
    GrailIndex* grail = allocateGrail(hGraph->size,lablesCount);
    grail = intiGrailIndex(grail,hGraph->size,lablesCount);
    
    int i,j,neightbor;
    int startNode,hGNode;
    int* expandedNeightborsCount; // Table that keeps how many neighbors where expanded from each node of the hypergraph.
     
    int rankCounter;
   
    for(i=0; i<lablesCount; i++){
        
        expandedNeightborsCount = calloc(hGraph->size,sizeof(int)); 
        
        
        rankCounter = 1;
        
        
        for(j=0; j<hGraph->rootNodesCount; j++){
            
            startNode = hGraph->rootNodes[j];
            
            GStack* stack = createGStack();
            GStackNode* stackNodeParent ;
            GStackNode* stackNodeChild ;
            
            grail->tagsTable[startNode].lables[i].minRank = -1;
            
            int* value = malloc(2*sizeof(int));
            value[0] = startNode;
            value[1] = rankCounter;
            stackNodeChild = createGStackNode(value); // There is a need to keep a parent and a child node because the parent rank is affected from the child rank. 
            stackNodeParent = NULL;
            
            pushGStackNode(stack,stackNodeChild);
            
            
            
            while(!gStackIsEmpty(stack)){
                
                stackNodeChild = popGStackNode(stack);
                stackNodeParent = stack->top;
                
                hGNode = ((int*)(stackNodeChild->value))[0];
                                
                
                //neightbor =  chooseConsecutiveNodeNeightborToExpand(&(hGraph->hNodeTable[hGNode]),&(expandedNeightborsCount[hGNode]));
                neightbor =  chooseRandomNodeNeightborToExpand(&(hGraph->hNodeTable[hGNode]),&(expandedNeightborsCount[hGNode]));
                               
                if(neightbor == NO_OUTGOING_EDGE){ // Node doesn't have any neighbors to expand.
                    
                    grail->tagsTable[hGNode].lables[i].minRank=rankCounter;
                    grail->tagsTable[hGNode].lables[i].rank=rankCounter;
                    
                    
                    
                    if(stackNodeParent!=NULL){ // Parent Node needs to get the minRannk{all his children}
                        
                        int parrentMinRank = ((int*)(stackNodeParent->value))[1];
                        
                    
                        if(parrentMinRank==-1 ){  // Prosoxh
                            ((int*)(stackNodeParent->value))[1] = rankCounter;
                        }
                    }
                    rankCounter++;
                    free(stackNodeChild->value);
                    free(stackNodeChild);
                
                }
                else if(neightbor == ALL_NEIGHTBORS_EXPANDED){
                    
                    int childMinRank = ((int*)(stackNodeChild->value))[1]; //Finally the childNode min rank is passed in the grail .
                    
                    grail->tagsTable[hGNode].lables[i].minRank = childMinRank  ;
                    grail->tagsTable[hGNode].lables[i].rank = rankCounter;
                    if(stackNodeParent!=NULL){
                        //int hGNodeTemp = ((int*)(stackNodeParent->value))[0];
                        int parrentMinRank = ((int*)(stackNodeParent->value))[1];
                    
                        if(parrentMinRank==-1 || childMinRank < parrentMinRank){  // Parent Node needs to get the minRannk{all his children}
                            ((int*)(stackNodeParent->value))[1] = childMinRank;
                        }
                    }
                    rankCounter++;
                    free(stackNodeChild->value);
                    free(stackNodeChild);
                }
                else{
                    
                    if(grail->tagsTable[neightbor].lables[i].minRank==-1){ // If the expanded neighbor has not yet been visited. 
                        value = malloc(2*sizeof(int));
                        value[0] = neightbor;
                        value[1] = -1;
                        pushGStackNode(stack,stackNodeChild);
                        stackNodeChild = createGStackNode(value);
                        pushGStackNode(stack,stackNodeChild);
                    }
                    else{ // If neightbor was visited the our current node minRank is set in accordance to minRank{all children (neighbors)}
                        int childMinRank = ((int*)(stackNodeChild->value))[1];
                        if((grail->tagsTable[neightbor].lables[i].minRank < childMinRank) || (childMinRank ==-1)){
                            ((int*)(stackNodeChild->value))[1] = grail->tagsTable[neightbor].lables[i].minRank ; 
                        }
                        pushGStackNode(stack,stackNodeChild);
                    }
                
                }
                
            }
            
            destroyGStack(stack);
        }
      
        free(expandedNeightborsCount);
         
    }
    
    return grail;
}

int isReachableGrailIndex(GrailIndex* grail, SCC* scc, uint32_t nodeS ,uint32_t nodeF){
    
    if(grail==NULL){
        printf("Grail not alloctaed.\n");
        return -1;
    }
    if(scc==NULL){
        printf("Strongly connected components not allocated ");
        return -1;
    }
    
    int i;
    int answer = MAYBE;
    StrongComponent* componentS = scc->id_belongs_to_component[nodeS];
    StrongComponent* componentF = scc->id_belongs_to_component[nodeF];
    
    if(componentS ==NULL || componentF ==NULL){
        printf("Something whent wrong with components.\n");
        return -1;
    }
    
    if(componentS->component_id == componentF->component_id){
        answer = YES;
        return answer;
    }
    
    Lable lableS ,lableF;
   
    for(i=0; i<grail->lableCount; i++){
        
        lableS = grail->tagsTable[componentS->component_id].lables[i];
        lableF = grail->tagsTable[componentF->component_id].lables[i];
        if(compareLables(lableS,lableF)==NO){
            answer = NO;
            break;
        }
    
    }
    
    return answer;
   
}

int compareLables(Lable lableS, Lable lableF){
  
    if(lableS.minRank > lableF.minRank){
        return NO;
    }
    if(lableS.rank < lableF.minRank){
        return NO;
    }
    
    return MAYBE;
    
}

int chooseStartNode(HyperGraph hyperGraph){
    
    int startNode = rand() % hyperGraph.size;
    if(startNode == 0){
        startNode = rand() % hyperGraph.size;
    }
    return startNode;
}

int chooseConsecutiveNodeNeightborToExpand(HyperGraphNode* hyperGraphNode, int* offset){
    
    
    int neightbor;
    
    if(hyperGraphNode->fillFactor == 0){
        return NO_OUTGOING_EDGE;
    }
    
    if(*offset == hyperGraphNode->fillFactor){
        return ALL_NEIGHTBORS_EXPANDED;
    }
    neightbor = hyperGraphNode->neightborTable[*offset];
    (*offset)++;
    return neightbor;
    
}

int chooseRandomNodeNeightborToExpand(HyperGraphNode* hyperGraphNode, int* offset){
    
    int neightbor;
    
    
    if(hyperGraphNode->fillFactor == 0){
        return NO_OUTGOING_EDGE;
    }
    
    if((*offset) == hyperGraphNode->fillFactor){
        return ALL_NEIGHTBORS_EXPANDED;
    }
    
    
    int randomPlace = rand()%(hyperGraphNode->fillFactor - (*offset));
    
    neightbor = hyperGraphNode->neightborTable[(*offset)+randomPlace];
    
    swapPlace(hyperGraphNode,*offset,randomPlace);
    (*offset)++;
    
    return neightbor;
    
}

void swapPlace(HyperGraphNode* hyperGraphNode, int offset, int randomPlace){
    
    int temp1;
    int temp2;
    
    temp1=hyperGraphNode->neightborTable[offset];
    temp2=hyperGraphNode->neightborTable[randomPlace+offset];
    hyperGraphNode->neightborTable[offset] = temp2;
    hyperGraphNode->neightborTable[randomPlace+offset] = temp1;
    
}

int destroyGrailIndex(GrailIndex* grail){
    
    int i;
    for(i=0; i<grail->size; i++){
            free(grail->tagsTable[i].lables);
        
    }
    free(grail->tagsTable);
    free(grail);
    return OK_SUCCESS;
}

GrailIndex* allocateGrail(int size,int lablesCount){
    
    int i;
    
    GrailIndex* grailIndex = malloc(1*sizeof(GrailIndex));
    
    grailIndex->tagsTable = malloc(size*sizeof(Tags));
    for(i=0; i<size; i++){
        grailIndex->tagsTable[i].lables = malloc(lablesCount * sizeof(Lable));
    }
    
    return grailIndex;
    
}

GrailIndex* intiGrailIndex(GrailIndex* grail,int size,int lablesCount){
    
    if(grail==NULL){
        printf("GrailIndex not allocated.\n");
        return NULL;
    }
    int i,j;
    
    grail->lableCount = lablesCount;
    grail->size = size;
    
    for(i=0; i<size; i++){
        for(j=0; j<lablesCount; j++){
            grail->tagsTable[i].lables[j].minRank = -1;
            grail->tagsTable[i].lables[j].rank = -1;
        }
    }
  
    
   
    
    return grail;
}


