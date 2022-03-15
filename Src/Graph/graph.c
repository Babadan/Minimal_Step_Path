#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "DatasetReader.h"
#include "Graph.h"
#include "GHash.h"


Graph* setBiggestNode(Graph* graph,uint32_t node){
    
    if(graph->biggestNode<node){
        graph->biggestNode= node;
    }
    return graph;
}

void printNeighbors(Graph* graph, int node){
    
    
    
    char* buff;
    list_node* ls;
    int i;
    
    printf("%ld --> ",*graph->nodeIndex->indexTable[node].bufferoffset);
    fflush(stdout);
    if((*graph->nodeIndex->indexTable[node].bufferoffset)!=-1){
        buff = (graph->buffer->buffer + (*graph->nodeIndex->indexTable[node].bufferoffset));
        ls =(list_node*) buff;
    
        while(ls!=NULL){
            for(i=0; i<ls->fillFactor; i++){
                printf("%u ", ls->neighbor[i]);
            }
            ls = (list_node*)ls->nextListNode;
        
        }
    }
}

void printGraph(Graph* graph){
    
    NodeIndex* nodeIndex = graph->nodeIndex;
    list_node* nodeList;
    long int i,j ;
    
    for(j=0; j<nodeIndex->size; j++){
        if(nodeIndex->indexTable[j].bufferoffset[0]!=-1){
            
            nodeList = (list_node*) (graph->buffer->buffer + nodeIndex->indexTable[j].bufferoffset[0]);
            if(nodeList->edgeProperty[0]>0){
                printf("%ld -->  ",j);
            }
            else{
                printf("%ld <--  ",j);
            }
           
            while(nodeList!=NULL){
                for(i=0; i<nodeList->fillFactor; i++){

                    //printf("%u-%u , ",nodeList->neighbor[i],nodeList->edgeProperty[i]);
                    printf("%u, ",nodeList->neighbor[i]);
                    fflush(stdout);

                }

               nodeList = (list_node*) nodeList->nextListNode;
            }
            printf("\n\n");
        }
    }
    
}

int destroyGraph(Graph* graph){
    
    if(graph==NULL){
        perror("Trying to destroy Null pointer (graph).\n ");
    }
    else if(graph->nodeIndex==NULL){
        perror("Trying to destroy Null pointer (graph).\n ");
    }
    else if(graph->buffer==NULL){
        perror("Trying to destroy Null pointer (graph).\n ");
    }
    
    
    destroyBuffer(graph->buffer);
    destroyNodeIndex(graph->nodeIndex);
    
    free(graph);
    return OK_SUCCESS;
    
}

DoubleGraph graphCreate(FILE* fileGraph){
    
    DoubleGraph doubleGraph;
    char buff[CHUNK+1];
    uint32_t** partGraph;
    
    
    
    uint32_t* dissectedPart;
    long int bytesRead = 1 ;
    long int lines,counter;
    
   
    
    doubleGraph.graphIngoing = malloc(sizeof(Graph));
    doubleGraph.graphOutgoing = malloc(sizeof(Graph));
    doubleGraph.graphIngoing->biggestNode = -1;
    doubleGraph.graphOutgoing->biggestNode = -1;
    doubleGraph.graphIngoing->inOut=1;
    doubleGraph.graphOutgoing->inOut=0;
    doubleGraph.scc=NULL;
    doubleGraph.cc=NULL;
    
    
    doubleGraph.visitedTable = NULL;
    
    doubleGraph.graphIngoing->nodeIndex = createNodeIndex();
    doubleGraph.graphIngoing->buffer = createBuffer();
    
    
    doubleGraph.graphOutgoing->nodeIndex = createNodeIndex();
    doubleGraph.graphOutgoing->buffer = createBuffer();
    
    
    
    
    bytesRead = myfread(fileGraph ,buff);
    
    
    uint32_t c = 0;
    
    while(bytesRead!=EOF){
        
        lines = countNewLine(buff ,bytesRead);
        partGraph = graphChunkIntoArray(buff,bytesRead);
        
        c+=lines;
        
        counter=0;
        while(lines!=counter){
            dissectedPart=dissectChunkOutgoing(partGraph + counter,lines-counter); // Dissected part keeps all the neighbors that a node has in the part loaded from file 
           
            counter+=dissectedPart[0];
            //doubleGraph.graphOutgoing=setBiggestNode(doubleGraph.graphOutgoing,dissectedPart[1]);
            

            //check_resize(nodeIndexOut,dissectedPart[1]);
            insertNeighbors(doubleGraph.graphOutgoing,dissectedPart);
                           
            free(dissectedPart);
        }
        
        
        
       counter=0;
       while(lines!=counter){
            dissectedPart=dissectChunkIngoing(partGraph+counter,lines-counter);
            counter+=dissectedPart[0];
            //doubleGraph.graphIngoing=setBiggestNode(doubleGraph.graphIngoing,dissectedPart[1]);
            
            //check_resize(nodeIndexIn,dissectedPart[1]);
            insertNeighbors(doubleGraph.graphIngoing,dissectedPart);
                   
            free(dissectedPart);
        }
        
       
        destroyGraphArray(partGraph,lines);
       
        bytesRead = myfread(fileGraph ,buff);
       
    }
    
    //printGraph(doubleGraph.graphIngoing);
    //printGraph(doubleGraph.graphOutgoing);
    //destroyGraph(doubleGraph.graphIngoing);
    //destroyGraph(doubleGraph.graphOutgoing);
    
    return doubleGraph;
 
}

int connectIndexBuffer(Graph* graph,uint32_t nodeId){ // If the node is new then we establish connection between IndexTableCell and the buffer. 
    long int bufferoff;
    Index* indexTable;
    
    if(graph->nodeIndex==NULL){
        printf("NodeIndex is not allocate.\n");
        return -1;
    }
    
    if(graph->nodeIndex->indexTable == NULL){
            printf("indexTable not allocated\n");
            return -1;
    }
    
    bufferoff = graph->nodeIndex->indexTable[nodeId].bufferoffset[0];
    
    if(bufferoff!=-1){
        return 0;
    }
    
    while(!testBufferCapacity(graph->buffer,sizeof(list_node))){       
        resizeBuffer(graph->buffer);     
    }
    
    indexTable = graph->nodeIndex->indexTable;
    indexTable[nodeId].bufferoffset[0] = getOffset(graph->buffer);
    indexTable[nodeId].tail = getOffset(graph->buffer);
    if(graph->inOut==0){
        indexTable[nodeId].hashTable= malloc(1*sizeof(HashTable));
        initHashTable2(indexTable[nodeId].hashTable,HASHSTARTSIZE,BUCKETSIZE);
    }
    
    allocateNewNode(graph->buffer);
    
    return OK_SUCCESS;
        
    
}

int insertNeighbors(Graph* graph, uint32_t* newNeighborTable){ // Works
    
    
    if(newNeighborTable==NULL){
        printf("Neighbor table is NULL\n");
        return -1;
    }
    
    if(graph->buffer==NULL){
        printf("ListNode is NULL\n");
        return -1;
    }
    
    list_node* listNode ;
//    char* bufferPtr;
    char* newbufferPtr;
    int i;
    Entry* entry;
    
    // if The node that we insert is bigger that the current size of the indexTable the we need to expand the indexTable
    graph=setBiggestNode(graph,newNeighborTable[1]);
            
    check_resize(graph->nodeIndex,newNeighborTable[1]);

//--------------------------------------------------------------------------------------------    
    
    list_node* newlistNode;
        
    connectIndexBuffer(graph,newNeighborTable[1]);
    listNode = getListTail(graph->nodeIndex->indexTable[newNeighborTable[1]],graph->buffer->buffer); 
    
    uint32_t newNeighborSize = newNeighborTable[0]; //Get number of new neighbors of the neighbor table 
        
    int nFill = listNode->fillFactor;
    for(i=0; i<newNeighborSize; i++){
        
        //Check for double edge . Only for the outGoing Graph. For reducing memory .
        if(graph->inOut==0 && getEntry(graph->nodeIndex->indexTable[newNeighborTable[1]].hashTable, newNeighborTable[i+3])!=NULL)
            continue;
       
        
        if(nFill == NEIGHBOR_SIZE_ARRAY){ // Allocate new node if the buffersNode neighborTable is full
            nFill=0;
             
            while(!testBufferCapacity(graph->buffer,sizeof(list_node))){
                resizeBuffer(graph->buffer);
                listNode = getListTail(graph->nodeIndex->indexTable[newNeighborTable[1]],graph->buffer->buffer);
            }
           
            newbufferPtr = allocateNewNode(graph->buffer);
            newlistNode = getListNode(newbufferPtr);
            newlistNode->fillFactor = nFill;
            listNode->nextListNode = newbufferPtr;
            listNode=newlistNode;
            graph->nodeIndex->indexTable[newNeighborTable[1]].tail = (long int)((char*)(listNode) - graph->buffer->buffer);
                        
        }
                
        if(graph->inOut==0){
            entry = malloc(1*sizeof(Entry));
            entry->key = newNeighborTable[i+3];
            
            
            long int* offs = malloc(1*sizeof(long int)); 
            *offs = (long int)((char*)(listNode) - graph->buffer->buffer);
            entry->pointer = (void*)offs;
            
            addToHashTable(graph->nodeIndex->indexTable[newNeighborTable[1]].hashTable,entry);
            
        }
        
        
        listNode->neighbor[nFill] = newNeighborTable[i+3];
        listNode->edgeProperty[nFill] = newNeighborTable[2];
        nFill++;
        listNode->fillFactor = nFill;
       
    }
    return OK_SUCCESS;
}

uint32_t* dissectChunkOutgoing(uint32_t** partGraph , long int size){
    
    if(partGraph==NULL){
        printf("The is no Graph Table to dissect\n");
        return NULL;
    }
    
    uint32_t sample = partGraph[0][0];
    uint32_t* dissectedPart;
    long int dissectionSize = 0;
    long int i;
    
   
    
    for(i=0; i<size; i++){
        if(partGraph[i][0]!=sample){
            break;      
        } 
        dissectionSize++;
    }

    
    dissectedPart = malloc((dissectionSize+3)*sizeof(uint32_t)); //Prosoxh diaxeirizetai pinakes mege8ous mexri uint mege8ous
    
    dissectedPart[0]=dissectionSize;
    dissectedPart[1]=sample;
    dissectedPart[2]=OUTGOING_EDGE;
    
    for(i=0; i<dissectionSize;i++){
        dissectedPart[i+3]=partGraph[i][1];
    }
    
    
    
    return dissectedPart;
    
}

uint32_t* dissectChunkIngoing(uint32_t** partGraph , long int size){
    
    if(partGraph==NULL){
        printf("The is no Graph Table to dissect\n");
        return NULL;
    }
    
    uint32_t sample = partGraph[0][1];
    uint32_t* dissectedPart;
    long int dissectionSize = 0;
    long int i;
    
    for(i=0; i<size; i++){
        if(partGraph[i][1]!=sample){
            break;      
        } 
        dissectionSize++;
    }

    
    dissectedPart = malloc((dissectionSize+3)*sizeof(uint32_t)); //Prosoxh diaxeirizetai pinakes mege8ous mexri uint mege8ous
    
    dissectedPart[0]=dissectionSize;
    dissectedPart[1]=sample;
    dissectedPart[2]=INGOING_EDGE;
    
    for(i=0; i<dissectionSize;i++){
        dissectedPart[i+3]=partGraph[i][0];
    }
    
    
    
    return dissectedPart;
    
}
