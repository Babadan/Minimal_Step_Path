#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "Buffer.h"
#include <assert.h>
#include "NodeIndex.h"
#include <stdbool.h>


int nodeList_error = OK; 

NodeIndex* createNodeIndex(){
    NodeIndex* nodeIndex;
    
    nodeIndex = malloc(1*sizeof(NodeIndex));
    long int i;
    
    if(nodeIndex == NULL){
        printf("Something went wrong with malloc. %s \n ",strerror(errno));
        nodeList_error = FAIL_MEM_ALLOCATION;
        return NULL;
    }
    nodeIndex->indexTable = calloc(INDEX_START_SIZE,sizeof(Index));
    if(nodeIndex->indexTable == NULL){
        printf("Something went wrong with malloc. %s \n ",strerror(errno));
        nodeList_error = FAIL_MEM_ALLOCATION;
        return NULL;
    }
    
    for(i=0; i<INDEX_START_SIZE; i++){
        nodeIndex->indexTable[i].bufferoffset = malloc(sizeof(long int));
        nodeIndex->indexTable[i].bufferoffset[0]= -1;
        nodeIndex->indexTable[i].tail= -1;
        nodeIndex->indexTable[i].hashTable=NULL;
    }
    
    nodeIndex->size = INDEX_START_SIZE ;
    
    return nodeIndex;
    
}

int destroyNodeIndex(NodeIndex* nodeIndex){
    
    if(nodeIndex==NULL){
        printf("NodeIndex is not allocate.");
        nodeList_error = NODEINDEX_NOT_ALLOCATE;
        return -1;
    }
    
    if(nodeIndex->indexTable==NULL){
        free(nodeIndex);
        return OK_SUCCESS;
    }
    int i;
    for(i=0; i<nodeIndex->size; i++){
        if(nodeIndex->indexTable[i].hashTable==NULL){
            free(nodeIndex->indexTable[i].bufferoffset);
            continue;
        }
            
        destroyHashEntriesValues(nodeIndex->indexTable[i].hashTable);
        free(nodeIndex->indexTable[i].hashTable);
        free(nodeIndex->indexTable[i].bufferoffset);
    }
    
    free(nodeIndex->indexTable);
    
    free(nodeIndex);
    
    return OK_SUCCESS;    
}

long int getListHead(NodeIndex* nodeIndex , uint32_t nodeId){
    
    if(nodeIndex==NULL){
        printf("NodeIndex is not allocate.\n");
        nodeList_error = NODEINDEX_NOT_ALLOCATE;
        return -1;
    }
    
    Index* indexTable = nodeIndex->indexTable;
    
    if(indexTable == NULL){
            printf("Index table not allocated\n");
            nodeList_error = INDEX_TABLE_NOT_ALLOCATED;
            return -1;
    }
    
    if(nodeIndex->indexTable[nodeId].bufferoffset[0]==-1){
        nodeList_error=GRAPHNODE_NOT_FOUND;
        return -1;
    }
    
    return nodeIndex->indexTable[nodeId].bufferoffset[0];
     
}

int check_resize(NodeIndex* nodeIndex, uint32_t nodeId){ // Check if the size of the Index is smaller that the nodeId if it is resizes the IndexTable
    
    Index* newIndexTable;
    long int i ;
    
    long int newSize,oldSize;
    
    if(nodeIndex==NULL){
        printf("NodeIndex is not allocate.\n");
        nodeList_error = NODEINDEX_NOT_ALLOCATE;
        return -1;
    }
    
    if(nodeIndex->indexTable==NULL){
        printf("Index table not allocated\n");
        nodeList_error = INDEX_TABLE_NOT_ALLOCATED;
        return -1;
    }
    
    if(nodeId >= nodeIndex->size){
        
        newSize = nodeId + 1 + nodeId*10/100;
        oldSize = nodeIndex->size;
       
        
        newIndexTable = calloc(newSize,sizeof(Index));
        
        memcpy(newIndexTable,nodeIndex->indexTable,nodeIndex->size*sizeof(Index));
        nodeIndex->size=newSize;
        free(nodeIndex->indexTable);
        nodeIndex->indexTable=newIndexTable;
        
       
        
        for(i=newSize-1; i>oldSize-1; i--){
            nodeIndex->indexTable[i].bufferoffset = malloc(sizeof(long int));
            nodeIndex->indexTable[i].bufferoffset[0] = -1;
            nodeIndex->indexTable[i].hashTable=NULL;
        }
        
        return 0; 
    }
   
    return 1;
}

void NodeIndexError(char* errString){
    printf("%s: ", errString);
    
    switch (nodeList_error) {
        case OK:
            perror("EVERYTHING IS OK");
            break;
        case FAIL_MEM_ALLOCATION:
            perror("FAILD MEMORY ALLOCATION");
            break;
        case BUFFER_NOT_ALLOCATED:
            perror("BUFFER IS NOT ALLOCATED");
            break;
        case NODEINDEX_NOT_ALLOCATE:
            perror("NODEINDEX IS NOT ALLOCATE");
            break;
        case FAIL_TO_ALLOCATE_NODEINDEXNODE:
            perror("FAIL TO ALLOCATE NODEINDEXNODE");
            break;
    }
}

list_node* getListTail(Index index,char* bufferStart){
    if(bufferStart==NULL){
        printf("Buffer not Allocated\n");
        return NULL;
    }
    
    list_node* ls;
    ls =(list_node*)(bufferStart + index.tail);
    return ls;
    
}