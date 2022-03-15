#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "GHash.h"
#include "Graph.h"
#include "Buffer.h"
#include "HyperGraph.h"
#include "NodeIndex.h"
#include "StronglyConnectedComponents.h"

HyperGraph* createHyperGraph(DoubleGraph* doubleGraph ,SCC* scc){
    
    Graph* graph = doubleGraph->graphOutgoing;
    int biggestNode = max(doubleGraph->graphIngoing->biggestNode,doubleGraph->graphOutgoing->biggestNode);
    
    if(graph==NULL){
        printf("Graph not allocated.\n");
        return NULL;
    }
    if(scc==NULL){
        printf("SCC is NULL.\n");
        return NULL;
    }
    
    int i,j;
    
    HyperGraph* hyperGraph = malloc(1*sizeof(HyperGraph));
    initHyperGraph(hyperGraph);
    
    hyperGraph->size = scc->components_count+1;
    
    if(hyperGraph->size==-1){
        printf("There are no Strongly connected Components.\n");
        free(hyperGraph);
        return NULL;
    }
    
    hyperGraph->hNodeTable = malloc(hyperGraph->size*sizeof(HyperGraphNode));
    int neightborTableSize = NEIGHTBOR_TABLE_START_SIZE ;
    for(i=0; i<scc->components_count; i++){
        uint32_t cId = scc->components[i].component_id;
        hyperGraph->hNodeTable[cId].nodeId = &(scc->components[i]);
        hyperGraph->hNodeTable[cId].neightborTable = calloc(neightborTableSize,sizeof(int));
        hyperGraph->hNodeTable[cId].tableSize = neightborTableSize;
        hyperGraph->hNodeTable[cId].fillFactor = 0;
        
        initHashTable(&(hyperGraph->hNodeTable[cId].hashTable),1,6);
    }
    
    
    
    list_node* listNode;
    long int nodeOffset;
    uint32_t neightbor;
    
    
    StrongComponent* neighboringComponent;
    StrongComponent* component;
    
    bool* rootNodeMap = calloc(scc->components_count+1,sizeof(bool));
    int countRootNodes=scc->components_count;
    
    for(i=0; i<biggestNode+1; i++){
        nodeOffset = getListHead(graph->nodeIndex,i);
        if(nodeOffset==-1)
            continue;
        listNode = (list_node*) (graph->buffer->buffer + nodeOffset);
        while(listNode!=NULL){
            for(j=0; j<listNode->fillFactor; j++){
                neightbor = listNode->neighbor[j];
                neighboringComponent = scc->id_belongs_to_component[neightbor];
                component = scc->id_belongs_to_component[i];
                if(component->component_id != neighboringComponent->component_id){
                    //printf("%d\n",i);
                    insertEdge(hyperGraph ,component, neighboringComponent);
                    
                    if(rootNodeMap[neighboringComponent->component_id]==false){ // Finding root nodes 
                        rootNodeMap[neighboringComponent->component_id] = true;
                        countRootNodes--;
                    }
                }
            }
            listNode =(list_node*) (listNode->nextListNode);
        }
    }
    
    hyperGraph->rootNodes = malloc(countRootNodes*sizeof(int));
    
    hyperGraph->rootNodesCount = countRootNodes;
    i=0;
    for(j=1; j<scc->components_count+1; j++){
        if(rootNodeMap[j]==false){
            hyperGraph->rootNodes[i]=j;
            i++;
        }
    }
    
    
    
    free(rootNodeMap);
    
    return hyperGraph;
    
}

void printRootNodes(HyperGraph* hyperGraph){
    int i; 
    printf("RootNode : ");
    for(i=0; i<hyperGraph->rootNodesCount; i++){
        printf("%d ",hyperGraph->rootNodes[i]);
    }
    printf("\n");
}

HyperGraphNode* checkResizeNeightborTable(HyperGraphNode* hyperGraphNode){
    
    if(hyperGraphNode==NULL){
        printf("HGraph node not allocated.\n");
        return NULL;
    }
    
    int newSize,oldSize,i;
    
    if(hyperGraphNode->tableSize == hyperGraphNode->fillFactor ){
        newSize = hyperGraphNode->tableSize*2;
        oldSize =  hyperGraphNode->tableSize;
        hyperGraphNode->neightborTable = realloc(hyperGraphNode->neightborTable,newSize*sizeof(int));
        for(i=oldSize; i<newSize; i++){
            hyperGraphNode->neightborTable[i] = 0;
        }
        hyperGraphNode->tableSize = newSize;
    }
    
    return hyperGraphNode;
    
}

int destroyHyperGraph(HyperGraph* hyperGraph){
    
    int i;
    for(i=1; i<hyperGraph->size; i++){
        //destroyHashEntries(&(hyperGraph->hNodeTable[i].hashTable)); 
        free(hyperGraph->hNodeTable[i].neightborTable);
    }
    free(hyperGraph->hNodeTable);
    free(hyperGraph->rootNodes);
    free(hyperGraph);
    return OK_SUCCESS;
}

bool checkDublicateEdge(HyperGraph* hyperGraph ,StrongComponent* compS, StrongComponent* compF){
    
    HyperGraphNode* hyperGraphNode = &(hyperGraph->hNodeTable[compS->component_id]);
    StrongComponent* neightboringComponent;
    
    neightboringComponent = (StrongComponent*) getEntry(&(hyperGraphNode->hashTable),compF->component_id);
    
    if(neightboringComponent!=NULL){
        return true;
    }
    return false;
}

int insertEdge(HyperGraph* hyperGraph ,StrongComponent* compS, StrongComponent* compF ){
    
    HyperGraphNode* hyperGraphNode = &(hyperGraph->hNodeTable[compS->component_id]);
        
    if(checkDublicateEdge(hyperGraph ,compS,compF))
        return DOUBLE_EDGE;
    
    hyperGraphNode = checkResizeNeightborTable(hyperGraphNode);
    int fillFactor = hyperGraphNode->fillFactor; 
    
    hyperGraphNode->neightborTable[fillFactor] =compF->component_id; 
    
    
    
    hyperGraphNode->fillFactor++;
    
    Entry* entry = malloc(1*sizeof(Entry));
    initEntry(entry);
    entry->key = compF->component_id;
    entry->pointer = (void*) compF;
    
    addToHashTable(&(hyperGraphNode->hashTable),entry);
    return EDGE_INSERTED;
}

int destroyHyperHashTables(HyperGraph* hyperGraph){
    
    int i;
    for(i=1; i<hyperGraph->size; i++){
        destroyHashEntries(&(hyperGraph->hNodeTable[i].hashTable)); 
    }
    
    return OK_SUCCESS;
        
}

HyperGraph* initHyperGraph(HyperGraph* hyperGraph){
    
    if(hyperGraph==NULL){
        printf("HyperGraph not allocated.\n");
        return NULL;
    }
    
    hyperGraph->hNodeTable=NULL;
    hyperGraph->size=-1;
    hyperGraph->rootNodes =NULL;
    hyperGraph->rootNodesCount = 0;
    
    
    return hyperGraph;
    
}

void printHyperGraph(HyperGraph* hyperGraph){
    int i,j;
    for(i=1; i<hyperGraph->size; i++){
        printf("%u--> ",hyperGraph->hNodeTable[i].nodeId->component_id);
        j=0;
        
        while(hyperGraph->hNodeTable[i].fillFactor>j ){
           
            
            printf("%d ",hyperGraph->hNodeTable[i].neightborTable[j]);
            j++;
        }
        printf("\n");
    }
}
