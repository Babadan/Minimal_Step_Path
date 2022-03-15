#include <stdio.h>
#include <stdlib.h>
#include "GQueue.h"
#include "GList.h"


GQueue* createGQueue(){
    GQueue* queue = malloc(1*sizeof(GQueue));
    queue->head=NULL;
    queue->tail=NULL;
    queue->size=0;
    return queue;
    
}




GQueueNode* createGQueueNode(){
    GQueueNode* queueNode = malloc(1*sizeof(GQueueNode));
    queueNode->value=NULL;
    queueNode->next =NULL;
    queueNode->previous=NULL;
    return queueNode;
}


int insertAtGQueue(GQueue* queue ,GQueueNode* queueNode){
    
    if(queue ==NULL){
        printf("List not allocated.\n");
        return -1;
    }
    if(queueNode==NULL){
        printf("List node not allocated.\n");
        return -1;
    }
    
    GQueueNode* qs;
    
    if(queue->tail==NULL){
        queue->head=queueNode;
        queue->tail=queueNode;
    }
    else{
        qs=queue->tail;
        queue->tail->previous = queueNode;
        queue->tail=queueNode;
        queueNode->next = qs;
    }
    queue->size++;
    
    return 1;
}


GQueueNode* takefromGQueue(GQueue* queue){
    
    if(queue ==NULL){
        printf("List not allocated.\n");
        return NULL;
    }
    if(queue->head ==NULL){
        //printf("Queue is empty.\n");
        return NULL;
    }
    GQueueNode* qs = queue->head;
    
    if(qs->previous==NULL){
        queue->head=NULL;
        queue->tail=NULL;
    }
    else{
        
        queue->head = qs->previous;
        queue->head->next=NULL;
       
    }
    queue->size--;
    return qs;
    
    
}



int destroyGqueue(GQueue* queue){
    
    if(queue==NULL)
        return 0;
    
    GQueueNode* temp ;
    while(queue->head!=NULL){
        temp=queue->head;
        queue->head = temp->previous;
        if(temp->value!=NULL){
            free(temp->value);
        }
        free(temp);
    }
    free(queue);
    return 0;
    
    
}

int destroyGQueueNode(GQueueNode* queueNode){
    
    if(queueNode==NULL){
        printf("ListNodeNotAllocated.\n");
        return -1;
    }
    
    free(queueNode->value);
    free(queueNode);
    return 0;
}