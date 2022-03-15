#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Graph.h"
#include "math.h"
#include "WeaklyConectedComponents.h"
#include "GList.h"

CC* estimateConnectedComponets(DoubleGraph graph){
    
    unsigned int i ;
    uint32_t biggestNode;
    uint32_t biggestInNode;
    uint32_t biggestOutNode;
    uint32_t graphNodeId;
    uint32_t ccId=0;
    list_node* ls;
    Node* queue1Node;
    Node* queue2Node;
    long int bufferOffset;
    biggestOutNode = graph.graphOutgoing->biggestNode; 
    biggestInNode = graph.graphIngoing->biggestNode; 
    
    if(biggestOutNode > biggestInNode){
        biggestNode = graph.graphOutgoing->biggestNode;
    }
    else{
        biggestNode = graph.graphIngoing->biggestNode;
    }
    
    CC* cc = malloc(1*sizeof(CC));             
    cc->ccindex = calloc(biggestNode+1,sizeof(CC));
    cc->ccIndexSize = biggestNode+1;
    cc->metricVal = METRIC_VAL;
    
    cc->updateIndex=malloc(1*sizeof(UpdateIndex));
    cc->updateIndex = initUpdateIndex(cc->updateIndex);
    
    WeakComponent* wComponent ;
    
    List* componentList = createList();
    List* nodeList = createList();
    ListNode* componentListNode;
    ListNode* nodeListNode;
    
    Visited visitTable ;
    visitTable.visitMap = malloc((biggestNode+1)*sizeof(Node*));
     
    Queue* queue1 = createQueue();
    Queue* queue2 = createQueue();
    for(i=0; i<=biggestNode; i++){
        insertAtQueue(queue1,i);
        visitTable.visitMap[i]=queue1->tail;
    }
    
    while(queue1->head!=NULL){
        
        ccId++;
        graphNodeId = queue1->head->node;
        queue1Node = queue1->head;
        deleteNodeFromQueue(queue1,queue1Node);
        insertNodeAtQueue(queue2,queue1Node);
        
        wComponent = malloc(1*(sizeof(WeakComponent)));
        wComponent->component_id = ccId;
        wComponent->included_nodes_count = 0;
        
        queue2Node=takeNode(queue2);
       
        
        while(queue2Node!=NULL){
            
            cc->ccindex[graphNodeId] = ccId;
            
            // Inserting include node id in the weak component
            int* n = malloc(1*sizeof(int));
            *n = graphNodeId;
            nodeListNode = createListNode();
            nodeListNode->value = (void*) (n);
            insertAtList(nodeList,nodeListNode);
            wComponent->included_nodes_count++;
            
            if(biggestInNode>=graphNodeId){
                bufferOffset = graph.graphIngoing->nodeIndex->indexTable[graphNodeId].bufferoffset[0];

                if(bufferOffset!=-1){
                    ls =(list_node*) (graph.graphIngoing->buffer->buffer + bufferOffset);

                    while(ls!=NULL){
                        for(i=0; i<ls->fillFactor; i++){
                            if(cc->ccindex[ls->neighbor[i]]==0){
                                cc->ccindex[ls->neighbor[i]]=ccId;
                                queue1Node =visitTable.visitMap[ls->neighbor[i]];
                                deleteNodeFromQueue(queue1,queue1Node);
                                insertNodeAtQueue(queue2,queue1Node);
                                
                                // Inserting include node id in the weak component
                                n = malloc(1*sizeof(int));
                                *n = ls->neighbor[i];
                                nodeListNode = createListNode();
                                nodeListNode->value = (void*)(n);
                                insertAtList(nodeList,nodeListNode);
                                wComponent->included_nodes_count++;
                                
                                
                            }
                        }
                        ls = (list_node*)ls->nextListNode;
                    }
                }
            }
            if(biggestOutNode>=graphNodeId){
                bufferOffset = graph.graphOutgoing->nodeIndex->indexTable[graphNodeId].bufferoffset[0];

                if(bufferOffset != -1){
                    ls =(list_node*) (graph.graphOutgoing->buffer->buffer + bufferOffset);

                    while(ls!=NULL){
                        for(i=0; i<ls->fillFactor; i++){
                            if(cc->ccindex[ls->neighbor[i]]==0){ // Don't expand node that are already expanded
                                cc->ccindex[ls->neighbor[i]]=ccId;
                                queue1Node =visitTable.visitMap[ls->neighbor[i]];
                                deleteNodeFromQueue(queue1,queue1Node);
                                insertNodeAtQueue(queue2,queue1Node);
                                
                                // Inserting include node id in the weak component
                                n = malloc(1*sizeof(int));
                                *n = ls->neighbor[i];
                                nodeListNode = createListNode();
                                nodeListNode->value = (void*) (n);
                                insertAtList(nodeList,nodeListNode);
                                wComponent->included_nodes_count++;
                            }
                        }
                        ls = (list_node*)ls->nextListNode;
                    }
                }
            }
            free(queue2Node);
            queue2Node = takeNode(queue2);
            if(queue2Node!=NULL){
                graphNodeId = queue2Node->node;
            }
        }
        
        uint32_t listSize = nodeList->size ;
        
        wComponent->included_node_ids = malloc(listSize*sizeof(uint32_t));
        
        //Inserting in the component List
        for(i=0; i<listSize; i++){
            nodeListNode = takefromListEnd(nodeList);
            wComponent->included_node_ids[i] = *((int*)nodeListNode->value);
            destroyListNode(nodeListNode);
        }
        componentListNode = createListNode();
        componentListNode->value = (void*) (wComponent);
        insertAtList(componentList,componentListNode);
       
    }
    
    // We are using List to temporary store weakComponents so we know the exact size to allocate .
    uint32_t listSize = componentList->size ;
        
    cc->component = malloc((listSize+1)*sizeof(WeakComponent*));
    for(i=0; i<listSize; i++){
        componentListNode = takefromListEnd(componentList);
        cc->component[i] = ((WeakComponent*)componentListNode->value);
        free(componentListNode);
       
    }
    cc->componentsCount = ccId;
    
    destroyList(componentList);
    destroyList(nodeList);
    
    printf("CC is %u\n",ccId);
    destroyQueue(queue1);
    destroyQueue(queue2);
    free(visitTable.visitMap);
   // free(cc->ccindex);
    //free(cc);
    return cc;
}

UpdateIndex* initUpdateIndex(UpdateIndex* upIn){
    
    upIn->dHash = malloc(1*sizeof(HashTable));  
    upIn->valueQueue = createGQueue();
    upIn->timesUsedCounter = 0;
    upIn->versioningHash = malloc(1*sizeof(HashTable));
    
    initHashTable2(upIn->dHash, UPDATE_INDEX_START_SIZE, 1);
    initHashTable2(upIn->versioningHash, UPDATE_INDEX_START_SIZE, 1);
    
    
    return upIn;
}

int insertNewEdge(CC* components,Job* job){
    
    uint32_t compIdS;
    uint32_t compIdE;
    
    compIdS = findNodeConnectedComponentID(components,job->nodeS);
    compIdE = findNodeConnectedComponentID(components,job->nodeF);
    
    if(compIdS == compIdE){
        return OK_SUCCESS;
    }
    
    //Creating string key for the Updates Index version table.
    //Key is the concatenation of the two weak components with dash in between. 
    char* key1 = createVersionHashKey(compIdS,compIdE);
    
    //If two components have been merged the version stays the same the old one.
    if(getEntryStringKey(components->updateIndex->versioningHash,key1)==-1){
        Entry* entry1 = malloc(1*sizeof(Entry));
        entry1->key = job->versioning; 
        entry1->pointer = key1;
        addToHashTableStringKey(components->updateIndex->versioningHash,entry1);
        
        
    }
    else{
        free(key1);
    }
    
    
    uint32_t minimumComponent = findMinComp(compIdS,compIdE);
       
    int* valueS = getEntry(components->updateIndex->dHash,compIdS);
    int* valueE = getEntry(components->updateIndex->dHash,compIdE);
    
    if(valueS == NULL && valueE == NULL){ // check if we have the same key
        uint32_t* minComp1 = malloc(1*sizeof(uint32_t));
        Entry* entryS = malloc(1*sizeof(Entry));
        (*minComp1) = minimumComponent;
        entryS->key = compIdS;
        entryS->pointer = minComp1;
        addToHashTable(components->updateIndex->dHash, entryS);
        
        //uint32_t* minComp2 = malloc(1*sizeof(uint32_t));
        Entry* entryE = malloc(1*sizeof(Entry));
        (*minComp1) = minimumComponent;
        entryE->key = compIdE;
        entryE->pointer = minComp1;
        addToHashTable(components->updateIndex->dHash, entryE);
        
        GQueueNode* qNodeValueUpadeteIndex =  createGQueueNode();
        
        qNodeValueUpadeteIndex->value = (void*)minComp1; 
        insertAtGQueue(components->updateIndex->valueQueue ,qNodeValueUpadeteIndex); 
    }
    else if(valueS == NULL && valueE!=NULL ){
        
        //uint32_t* minComp1 = malloc(1*sizeof(uint32_t));
        Entry* entryS = malloc(1*sizeof(Entry));
        //(*minComp1) = *(valueE);
        entryS->key = compIdS;
        entryS->pointer = valueE;
        addToHashTable(components->updateIndex->dHash, entryS);

        
    }
    else if(valueS != NULL && valueE == NULL){
        
        //uint32_t* minComp1 = malloc(1*sizeof(uint32_t));
        Entry* entryE = malloc(1*sizeof(Entry));
        //(*minComp1) = *(valueS);
        entryE->key = compIdE;
        entryE->pointer = valueS;
        addToHashTable(components->updateIndex->dHash, entryE);

    }
    else{
        int temp = findMinComp(*valueS,*valueE);
        (*valueS) = temp;
        (*valueE) = temp;
    }
    
    return OK_SUCCESS;
}

int nodesInSameComponent(CC* components ,Job* job,bool* updateIndexUsed ){
    
    if(components==NULL){
        return -1;
    }
    
    char* key1;
    char* key2;
    
    uint32_t nodeIdS = job->nodeS;
    uint32_t nodeIdE = job->nodeF;
    
    if(components->ccindex[nodeIdS]==components->ccindex[nodeIdE]){
        
        if(updateIndexUsed!=NULL)
            *updateIndexUsed = false;
        
        return SAME_WEAKLY_COMPONENT;
    }
    
    if(components->updateIndex==NULL || components->updateIndex->dHash==NULL || components->updateIndex->dHash->index == NULL ){
        printf("UpdateIndex not initialized .\n ");
        return -1;
    }
    
    int compIdS = components->ccindex[nodeIdS];
    int compIdE = components->ccindex[nodeIdE];
    
     
    key1 = createVersionHashKey(compIdS,compIdE);
    key2 = createVersionHashKey(compIdE,compIdS);
    
    
    int compVersion1 = getEntryStringKey(components->updateIndex->versioningHash,key1);
    int compVersion2 = getEntryStringKey(components->updateIndex->versioningHash,key2);
    
    free(key1);
    free(key2);
    
    if(compVersion1 <= job->versioning || compVersion2 <= job->versioning ){  // Checking Version of the two components .
        
        int* comp1 = getEntry(components->updateIndex->dHash,compIdS);
        int* comp2 = getEntry(components->updateIndex->dHash,compIdE);

        if(comp1==NULL || comp2 == NULL){
            if(updateIndexUsed!=NULL)
                *updateIndexUsed = false;
            return DIFFERENT_WEAKLY_COMPONENT;
        }

        if(updateIndexUsed!=NULL)
            *updateIndexUsed = true;

        if((*comp1) == (*comp2)){
            return SAME_WEAKLY_COMPONENT;
        }
    }
    return DIFFERENT_WEAKLY_COMPONENT;
}

int rebuildIndexes(CC* components){
    
    int i,j;
    Entry* entry;
    int hashSize = components->updateIndex->dHash->index_size;
    for(i=0; i<hashSize; i++){
        if(components->updateIndex->dHash->index[i].bucket->curr_size==0)
            continue;
        
        entry = components->updateIndex->dHash->index[i].bucket->entries[0];
        
        WeakComponent* comp = components->component[(entry->key)-1];
        for(j=0; j<comp->included_nodes_count; j++ ){
            int nodeId = comp->included_node_ids[j];
            components->ccindex[nodeId] = ((uint32_t*)entry->pointer)[0]; 
        }
        
    }
    
    destroyUpdateIndex(components->updateIndex);
    components->updateIndex=malloc(1*sizeof(UpdateIndex));
    components->updateIndex = initUpdateIndex(components->updateIndex);
    return OK_SUCCESS;
    
}

char* createVersionHashKey(uint32_t compIdS,uint32_t compIdE){
    
    char* key1 = calloc(24,sizeof(char));
    char* key2 = calloc(24,sizeof(char));
    
    sprintf(key1,"%u",compIdS);
    sprintf(key2,"%u",compIdE);
    
    
    int i;
    for(i=0; i<24; i++){
        if(key1[i]=='\0'){
            break;
        }
    }
    key1[i]='-';
    i++;
    
    int j;
    for(j=0; j<24; j++){
        if(key2[j]=='\0'){
            break;
        }
    }
    
    strncpy((key1+i),key2,j);
    
    free(key2);
    return key1;
    
}

int destroyConnectedComponents(CC* components){
    
    free(components->ccindex);
    
    int i;
    for(i=0; i<components->componentsCount; i++){
        destroyWeakComponent(components->component[i]);
    }
    free(components->component);
    destroyUpdateIndex(components->updateIndex);
    free(components);
    return OK_SUCCESS;
    
}

int destroyUpdateIndex(UpdateIndex* updateIndex){
    destroyHashEntries(updateIndex->dHash);
    destroyHashEntriesValues(updateIndex->versioningHash);
    destroyGqueue(updateIndex->valueQueue);
    free(updateIndex->dHash);
    free(updateIndex->versioningHash);
    free(updateIndex);
    return 0;
}

int destroyWeakComponent(WeakComponent* weakComponent){
    free(weakComponent->included_node_ids);
    free(weakComponent);
    return 1;
}

bool estimateMetric(double metric,uint32_t queriesUsedUpdateIndex,uint32_t queries){
    
    double met = (double)queriesUsedUpdateIndex/(double)queries;
    
    if(fabs(met - metric)<0.000000000001)
        return true;
    else{
        if(met<metric)
            return false;   
        else
            return true;
    }
    
}

uint32_t findMinComp(uint32_t compIdS,uint32_t compIdE){
    
    if(compIdS<compIdE){
        return compIdS;
    }
    else{
        return compIdE;
    }
}

uint32_t findNodeConnectedComponentID(CC* components, uint32_t nodeId){
    
    if(components==NULL || components->ccindex == NULL){
        return 0;
    }
    return components->ccindex[nodeId];
}

int printCCIndex(CC* components){
    
    uint32_t i; 
    for(i=0; i<components->ccIndexSize; i++){
        printf("%u --> %u\n",i,components->ccindex[i]);
    }
    
    return 1;
    
}