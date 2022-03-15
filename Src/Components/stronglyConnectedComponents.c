#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "StronglyConnectedComponents.h"
#include "Graph.h"
#include "GraphQA.h"
#include "Stack.h"
#include "Queue.h"
#include "GList.h"


int initSCC(SCC* scc){
    scc->components=NULL;
    scc->components_count=0;
    scc->id_belongs_to_component=NULL;
    scc->id_belongs_to_component= NULL;
    scc->id_belongs_size = 0;
    return 1;
}

void printInvertedIndex(SCC* scc){
    
    int i;
    int size = scc->id_belongs_size;
    for(i=0; i<size; i++){
        int comonent = findNodeStronglyConnectedComponentID(scc,i);
        printf("Id %d belongs to %d\n",i,comonent);
    }
    
    
}

bool sameComponent(SCC* scc,uint32_t nodeIdS, uint32_t nodeIdF ){
    
    uint32_t compIdS = scc->id_belongs_to_component[nodeIdS]->component_id;
    uint32_t compIdF = scc->id_belongs_to_component[nodeIdF]->component_id;
    if(compIdS == compIdF){
        return true;
    }
    return false;
    
}

SCC* createInvertedIndex(SCC* scc,uint32_t biggestNode){
    
    if(scc->components==NULL){
        printf("There are no components.\n");
        return NULL;
    }
    
    scc->id_belongs_to_component = malloc((biggestNode+1)*sizeof(StrongComponent*));
    scc->id_belongs_size = biggestNode+1;
    uint32_t i,j,poss;
    for(i=0; i<scc->components_count; i++){
        for(j=0; j<scc->components[i].included_nodes_count; j++){
            //printf("%u\n",j);
            poss=scc->components[i].included_node_ids[j];
            scc->id_belongs_to_component[poss]=&(scc->components[i]);
        }
        
    }
    
    return scc;
}

int destroySronglyConnectedComponents(SCC* scc){
    if(scc==NULL){
        printf("SCC is NULL\n");
        return -1;
    }
    if(scc->components==NULL){
        printf("SCC components are NULL\n");
        return -1;
    }
    uint32_t i;
    for(i=0; i<scc->components_count; i++){
        if(scc->components[i].included_node_ids==NULL){
            printf("NodeTable in component %u is NULL\n",i+1);
            return -1;
        }
        free(scc->components[i].included_node_ids);
    }
    free(scc->components);
    
    if(scc->id_belongs_to_component==NULL){
        printf("Inverted Index is NULL\n");
        return -1;
    }
    free(scc->id_belongs_to_component);
    free(scc);
    return OK_SUCCESS;
    
}

uint32_t max(uint32_t a , uint32_t b){
    
    if(a<b)
        return b;
    return a;
    
}

int findNodeStronglyConnectedComponentID(SCC* components, uint32_t node_Id){
    
    if(components==NULL){
        //printf("SCC is NULL.\n");
        return -1;
    }
    if(components->id_belongs_size < node_Id){
        printf("SCC node: %u doesn't exist.\n",node_Id);
        return -1;
    }
    
    return components->id_belongs_to_component[node_Id]->component_id;
}

int iterateStronglyConnectedComponentID(SCC* scc , ComponentCursor* cursor){
    
    if(scc==NULL){
        printf("SCC is NULL.\n");
        return -1;
    }
    
    if(scc->components==NULL){
        printf("Components Table is NULL");
        return -1;
    }
    if(cursor ==NULL){
        printf("Cursor not allocated");
        return -1;
    }
    
    int j;
    cursor->component_ptr = &(scc->components[0]);
    cursor->possition=0;
    bool flag = true;
    while(flag==true){
        
            printf("Component %u.\n",scc->components[cursor->possition].component_id);
            for(j=0; j<scc->components[cursor->possition].included_nodes_count; j++){
                printf("%u\n",scc->components[cursor->possition].included_node_ids[j]);
            }
            printf("-------------------------------------------------------\n\n");
        
        flag = next_StronglyConnectedComponentID(scc , cursor);
    }
    
    return OK_SUCCESS;
    
}

bool next_StronglyConnectedComponentID(SCC* scc , ComponentCursor* cursor){
    
    if(scc==NULL){
        printf("SCC is NULL.\n");
        return -1;
    }
    
    if(scc->components==NULL){
        printf("Components Table is NULL");
        return -1;
    }
    
    if(cursor ==NULL){
        printf("Cursor not allocated");
        return -1;
    }
    
    if(cursor->possition == scc->components_count-1)
        return false;
    
    int possition = cursor->possition+1;
    
    cursor->component_ptr = &(scc->components[possition]);
    cursor->possition = possition;
    
    return true;
    
}

SCC* estimateStronglyConnectedComponents(DoubleGraph* doubleGraph){
    
    if(doubleGraph==NULL){
        printf("DoubleGraph not allocated\n");
        return NULL;
    }
    
    Graph* graph = doubleGraph->graphOutgoing;
    
    if(graph==NULL){
        printf("Graph not allocated\n");
        return NULL;
    }
    
    uint32_t biggestNode = max(doubleGraph->graphIngoing->biggestNode,doubleGraph->graphOutgoing->biggestNode);
    
    //printf("sizeOflistNode = %ld.\n",sizeof(list_node) );
    
    SCC* scc = malloc(1*sizeof(SCC));
    initSCC(scc);
    
    uint32_t componentId = 1;
    int i,j;
    Node* nId1;
    Node* nId2;
    StackNode* tempStackNode;
   
    DefinedMap definedMap ;
    definedMap.map= calloc(biggestNode+1,sizeof(DefinedNode));
    
    
    uint32_t index = 1;
    StrongComponent* sComponent ;
    
    List* componentList = createList();
    ListNode* listNode;
    
    for(i=0; i<biggestNode+1; i++){
    //for(i=graph->biggestNode; i>=0; i--){
        
         
        if(definedMap.map[i].defined==true)
            continue;
       
        Stack* stack = createStack();
        Queue queue;
        StackNode* stackNode;
       
        stackNode= createStackNode(i);
        stackNode->index = index;
        stackNode->lowLink= index;
        stackNode->currentNeightbor = getNextNeighbor(*stackNode,*graph);


        definedMap.map[i].defined=true;
        definedMap.map[i].onStack=true;
        definedMap.map[i].stPtr=stackNode;
        pushStackNode(stack, stackNode);
        index++;

        
        initQueue(&queue);
        insertAtQueue(&queue,i);
        
        
        
        while(isEmpty(&queue)!=true){
            
            StackNode* st2 = stackNode;
            StackNode* st3 = NULL;
            
           
            
            while(stackNode->currentNeightbor!= ALL_NEIGHBORS_EXPANDED){
                if(definedMap.map[stackNode->currentNeightbor].defined==false){
                    stackNode = createStackNode(stackNode->currentNeightbor);
                    stackNode->index = index;
                    stackNode->lowLink= index;
                    stackNode->currentNeightbor = getNextNeighbor(*stackNode,*graph);

                    definedMap.map[stackNode->nodeId].defined=true;
                    definedMap.map[stackNode->nodeId].onStack=true;
                    definedMap.map[stackNode->nodeId].stPtr=stackNode;
                    pushStackNode(stack, stackNode);
                    index++;

                    insertAtQueue(&queue,stackNode->nodeId);
                }
                else if(definedMap.map[stackNode->currentNeightbor].onStack==true){
                    stackNode->lowLink = min(stackNode->lowLink,definedMap.map[stackNode->currentNeightbor].stPtr->index);
                    stackNode->currentNeightbor = getNextNeighbor(*stackNode,*graph);
                }
                else if(definedMap.map[stackNode->currentNeightbor].onStack==false){
                    stackNode->currentNeightbor = getNextNeighbor(*stackNode,*graph);
                }       
            }
            
            nId1 = takeFromTail(&queue);
            
            stackNode= definedMap.map[nId1->node].stPtr;
            free(nId1);
            nId2 = lookAtTail(&queue);
            if(nId2!=NULL){
                st2 = definedMap.map[nId2->node].stPtr;
                st2->lowLink = min(stackNode->lowLink,st2->lowLink);    
            }
            
            if(stackNode->index == stackNode->lowLink){
                int counter = 1;
                tempStackNode = stack->top;
                
                while(stackNode->nodeId!= tempStackNode->nodeId){ // Included_nodes_count
                    tempStackNode = tempStackNode->previous;
                    counter ++;
                }
                sComponent = malloc(1*sizeof(StrongComponent));
                sComponent->included_nodes_count = counter;
                sComponent->included_node_ids = malloc(counter*sizeof(uint32_t));
                
                j=0;
                do{
                    st3 = popStackNode(stack);
                    definedMap.map[st3->nodeId].onStack = false;
                    sComponent->included_node_ids[j]=st3->nodeId;
                    
                    j++;
                    if(st3->nodeId!=stackNode->nodeId)
                        free(st3);
                }while(stackNode->nodeId!=sComponent->included_node_ids[j-1]);
                
                free(st3);
                
                sComponent->component_id=componentId;
                sComponent->included_nodes_count=j;
                listNode = createListNode();
                listNode->value = (void*) sComponent; 
                componentId++;
                
                insertAtList(componentList,listNode);
            }
            
            stackNode = st2;   
        }
        
        destroyStack( stack);
    }
    
    scc->components = malloc(componentList->size*sizeof(StrongComponent));
    uint32_t listSize = componentList->size ;
    for(i=0; i<listSize; i++){
        listNode = takefromListEnd(componentList);
        scc->components[i] = *((StrongComponent*)listNode->value);
        destroyListNode(listNode);
    }
    destroyList(componentList);
    scc->components_count = (componentId-1);
    free(definedMap.map);
    createInvertedIndex(scc, biggestNode);
    printf("SCC are %u\n",scc->components_count);
    return scc;
}

uint32_t min(uint32_t a , uint32_t b){
    
    if(a<b)
        return a;
    return b;
}

long int getNextNeighbor(StackNode node,Graph graph){ // Getting the next neighbor from the hashTable of the Graph .
    
    
    list_node* ls;
    
    if(node.nodeId > graph.biggestNode){ // if node > biggest node that means that graph that node is in ingoing graph
        return -2;
    }
    
    if(node.currentNeightbor==-1){
        if(graph.nodeIndex->indexTable[node.nodeId].bufferoffset[0]!=-1){
            ls=(list_node*)(graph.buffer->buffer + graph.nodeIndex->indexTable[node.nodeId].bufferoffset[0]);
            if(ls->fillFactor!=0)
                return  ls->neighbor[0];
        }
        return ALL_NEIGHBORS_EXPANDED ;
    }
    
    long int* offset =(long int*) getEntry(graph.nodeIndex->indexTable[node.nodeId].hashTable,node.currentNeightbor);
    if(offset == NULL){
        printf("ERROR");
        printf("Node = %u Neighbor = %d \n",node.nodeId,node.currentNeightbor);
        exit(0);
        return 0;
    }
    
     
    ls = (list_node*)(graph.buffer->buffer + (*offset));
    int i;
    for(i=0; i<ls->fillFactor; i++){
        if(ls->neighbor[i]==node.currentNeightbor){
            break;
        }
    }
    
    if((i+1)==ls->fillFactor){
        if(ls->nextListNode==NULL){
            return ALL_NEIGHBORS_EXPANDED ;
        }
        else{
            ls=(list_node*)ls->nextListNode;
            return ls->neighbor[0];
        }
    }
    
    return ls->neighbor[i+1];
}

int estimateShortestPathStronglyConnectedComponents(SCC* stronglyConnectedComponents,DoubleGraph* doubleGraph ,uint32_t nodeS, uint32_t nodeF){
   
    SCC* scc;
    if(stronglyConnectedComponents==NULL)
        scc = doubleGraph->scc;
    else
        scc=stronglyConnectedComponents;
    
    int componentIdS = findNodeStronglyConnectedComponentID(scc,nodeS);
    int componentIdF = findNodeStronglyConnectedComponentID(scc,nodeF);
    
    if(componentIdS != componentIdF)
        return -1;
    
    
    int neightborComp;
   
    long int  i ;
    uint32_t distance=0;
    
    Queue* queueS = NULL ;
    Queue* queueF = NULL;
    
    list_node* neighborNodeS =NULL;
    list_node* neighborNodeF =NULL;
    
    long int  buffoffS;
    long int buffoffF;
     
    char* bufferPtrOut = doubleGraph->graphOutgoing->buffer->buffer;
    char* bufferPtrIn = doubleGraph->graphIngoing->buffer->buffer;
    
    NodeIndex* nodeIndexS = doubleGraph->graphOutgoing->nodeIndex;
    NodeIndex* nodeIndexF = doubleGraph->graphIngoing->nodeIndex ;
    
    if(nodeIndexS==NULL || nodeIndexF==NULL){
        perror("Node index not allocated something went wrong with graph loading\n");
    }
    
    if(nodeS > doubleGraph->graphOutgoing->biggestNode || nodeF>doubleGraph->graphIngoing->biggestNode){
        printf("Start or Final node doesn't exist in the graph. \n");
        return -1;
    }
    
    buffoffS = getListHead(nodeIndexS ,nodeS);
    buffoffF = getListHead(nodeIndexF ,nodeF);
    if(buffoffS == -1 || buffoffF == -1){
        return -1;
    }
    neighborNodeS =(list_node*) (bufferPtrOut+buffoffS);
    neighborNodeF =(list_node*) (bufferPtrIn +buffoffF);
    
    
    if(nodeS == nodeF){
        return distance;
    }
    
    
    char* visitedTable = doubleGraph->visitedTable;
    if(doubleGraph->graphOutgoing->biggestNode>doubleGraph->graphIngoing->biggestNode)
        visitedTable = calloc(sizeof(char),doubleGraph->graphOutgoing->biggestNode+1);
    else
        visitedTable = calloc(sizeof(char),doubleGraph->graphIngoing->biggestNode+1);
    
    queueS = createQueue();
    queueF = createQueue();
    
     
    Node* popedNodeS=NULL;
    Node* popedNodeF=NULL;
    
    int cCurrentLayerS=1;
    int cNextLayerS=0;
    int cCurrentLayerF=1;
    int cNextLayerF=0;

    
    insertAtQueue(queueS, nodeS);
    insertAtQueue(queueF, nodeF);
    
     visitedTable[nodeS] = S_VISITED ;
     visitedTable[nodeF] = F_VISITED ;
    
    while(!isEmpty(queueS) && !isEmpty(queueF)){
    //while(!isEmpty(queueS)){  
    //--------------------------------Step 1---------------------------------    
        distance++;
        while(cCurrentLayerS!=0){
            popedNodeS = takeNode(queueS);
            cCurrentLayerS--;

            buffoffS = getListHead(nodeIndexS ,popedNodeS->node);
            if(buffoffS==-1){
                neighborNodeS = NULL;
            }
            else{
                neighborNodeS =(list_node*)(bufferPtrOut + buffoffS);
            }
            
            while(neighborNodeS!=NULL){
                for(i=0; i<neighborNodeS->fillFactor; i++){
                    uint32_t neighborId = neighborNodeS->neighbor[i];
                    //printf("%u, ",neighborNodeS->neighbor[i]);
                    if(visitedTable[neighborId] == F_VISITED){
                        free(visitedTable);
                        free(popedNodeS);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                       // printf("%u, ",distance);
                        return distance;
                    }
                    
                    neightborComp = findNodeStronglyConnectedComponentID(scc,neighborId);
                    
                    if(visitedTable[neighborId] == NOT_VISITED && (neightborComp == componentIdS) ){
                        insertAtQueue(queueS,neighborId);
                        visitedTable[neighborId] = S_VISITED;
                        cNextLayerS++;
                    }
                }
                neighborNodeS =(list_node*) neighborNodeS->nextListNode;
            }
            //printf("\n");
            free(popedNodeS);
        }
        cCurrentLayerS = cNextLayerS;
        cNextLayerS = 0;
        
        distance++;
        while(cCurrentLayerF!=0){
            popedNodeF = takeNode(queueF);
            cCurrentLayerF--;

            buffoffF = getListHead(nodeIndexF ,popedNodeF->node);
            if(buffoffF==-1){
                neighborNodeF =NULL;
            }
            else{
                neighborNodeF =(list_node*)(bufferPtrIn + buffoffF);
            }

            nodeList_error=GRAPHNODE_NOT_FOUND;
            


            while(neighborNodeF!=NULL){

                for(i=0; i<neighborNodeF->fillFactor; i++){
                    uint32_t neighborId = neighborNodeF->neighbor[i];

                    if(visitedTable[neighborId] == S_VISITED){
                        free(visitedTable);
                        free(popedNodeF);
                        destroyQueue(queueS);
                        destroyQueue(queueF);
                        //printf("%u\n",distance);
                        return distance;
                    }
                    
                    neightborComp = findNodeStronglyConnectedComponentID(scc,neighborId);
                    
                    if(visitedTable[neighborId] == NOT_VISITED && (neightborComp == componentIdS)){
                        insertAtQueue(queueF,neighborId);
                        visitedTable[neighborId] = F_VISITED;
                        cNextLayerF++;
                    }
                }
                neighborNodeF =(list_node*) neighborNodeF->nextListNode;   
            }
            free(popedNodeF);
        }
        cCurrentLayerF = cNextLayerF;
        cNextLayerF = 0;
    }    
        
    free(visitedTable);
    destroyQueue(queueS);
    destroyQueue(queueF);
    
   // printf("%u\n",distance);
                   
    return -1;
      
}

int setIndexLowLink(StackNode* stackNode,uint32_t index,uint32_t lowLink){
    if(stackNode == NULL){
        printf("StackNode is NULL.\n");
        return -1;
    }
    stackNode->index =index;
    stackNode->lowLink = lowLink;
    return 1;
}

