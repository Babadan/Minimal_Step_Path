#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "Queue.h"

int error_Queue = OK;


int destroyQueue(Queue* queue){
    if(queue==NULL)
        return 0;
    
    Node* temp ;
    while(queue->head!=NULL){
        temp=queue->head;
        queue->head = temp->previous;
        free(temp);
    }
    free(queue);
    return 0;
}


bool isEmpty(Queue* queue){
    
    if(queue->head==NULL)
        return true;
    return false;
}

Queue* createQueue(){
    error_Queue = 0;
    
    Queue* q = malloc(sizeof(Queue));
    if(q ==NULL){
        error_Queue = FAIL_MEM_ALLOCATION;
    }
    q->head=NULL;
    q->tail=NULL;
    return q;
}

int initQueue(Queue* queue){
    if(queue==NULL){
        return -1;
    }
    
    queue->head=NULL;
    queue->tail=NULL;
    return 1;
}

Node* createNode(uint32_t nodeId){
    
    Node* node = malloc(1*sizeof(Node));
    if(node ==NULL){
        error_Queue = FAIL_MEM_ALLOCATION;
        return NULL;
    }
    
    node->next=NULL;
    node->previous=NULL;
    node->node = nodeId;
    
    return node;
}

void errorQueue(char* errString){
    printf("%s: ", errString);
    
    switch (error_Queue) {
        case OK:
            perror("EVERYTHING IS OK");
            break;
        case FAIL_MEM_ALLOCATION:
            perror("FAILD MEMORY ALLOCATION");
            break;
        case NODE_NOT_GIVEN:
            perror("NODE NOT GIVEN");
            break;
        case EMPTY_QUEUE:
            perror("EMPTYQUEUE");
            break;
        case NOT_ALLOCATED_QUEUE:
            perror("NOT ALLOCATED QUEUE");
            break;
        
    }
}

int deleteNodeFromQueue(Queue* queue,Node* node){
    
    if(queue==NULL){
        return -1;
    }
    if(node==NULL){
        return -1;
    }
    
    if(node->next==NULL && node->previous==NULL){
        queue->head=NULL;
        queue->tail=NULL;
    }
    else if(node->next==NULL && node->previous!=NULL){
        queue->head =node->previous;
        node->previous->next=NULL;
    }
    else if(node->next!=NULL && node->previous==NULL){
        queue->tail =node->next;
        node->next->previous=NULL;
    }
    else{
        node->next->previous =node->previous;
        node->previous->next =node->next;
    }
    
    node->next=NULL;
    node->previous=NULL;
    
    return 1;
    
}

Node* takeNode(Queue* queue){
    
    if(queue==NULL){
        printf("Queue is not allocate \n");
        error_Queue = NOT_ALLOCATED_QUEUE ;
        return NULL;
    }
    if(queue->head==NULL){
        //printf("Queue is Empty\n");
        error_Queue = EMPTY_QUEUE ;
        return NULL;
    }
    
    Node* node = queue->head;
    
    if(node->previous==NULL){
        queue->head=NULL;
        queue->tail=NULL;
    }
    else{
        queue->head=node->previous;
        node->previous->next=NULL;
    }
    return node; 
}

Node* takeFromTail(Queue* queue ){
    
    if(queue==NULL){
        printf("Queue not allocated.\n");
        return NULL;
    }
    if(queue->tail==NULL){
        //printf("Queue is empty.\n");
        return NULL;
    }
    Node* node=queue->tail;
    
    if(queue->tail->next==NULL){
        
        queue->head=NULL;
        queue->tail=NULL;
    }
    else{
        queue->tail->next->previous=NULL;
        queue->tail=queue->tail->next;
    }
    
    return node;
    
}

Node* lookAtTail(Queue* queue){
    if(queue==NULL){
        printf("Queue not allocated.\n");
        return NULL;
    }
    if(queue->tail==NULL){
        //printf("Queue is empty.\n");
        return NULL;
    }
    
    return queue->tail;
}

Node* looknNode(Queue* queue,int possition){
    int i;
    Node* node = NULL;
    if(possition <0 ){
    
        if(queue==NULL){
            printf("Queue not allocated.\n");
            return NULL;
        }
        if(queue->tail==NULL){
            printf("Queue is empty.\n");
            return NULL;
        }
        
        node = queue->tail;
        
        for(i=possition; i<-1; i++){
            node = node->next;
            if(node==NULL)
                break;
        }   
    }
    if(possition > 0 ){
        if(queue==NULL){
            printf("Queue not allocated.\n");
            return NULL;
        }
        if(queue->head==NULL){
            printf("Queue is empty.\n");
            return NULL;
        }
        
        node = queue->head;
        for(i=1; i<possition; i++){
            node = node->previous;
            if(node==NULL)
                break;
        }
    }
   
    
    return node;
}

int insertNodeAtQueue(Queue* queue,Node* node){
    
    if(queue==NULL){
        printf("Queue is not allocate \n");
        error_Queue = NOT_ALLOCATED_QUEUE ;
        return NOT_ALLOCATED_QUEUE;
    }
    if(node==NULL){
        printf("No node given to insert.\n");
        error_Queue = NODE_NOT_GIVEN ;
        return NODE_NOT_GIVEN;
    }
    
    if(queue->tail==NULL){
        queue->tail = node;
        queue->head = node;
        return OK;
    }
    
    queue->tail->previous = node;
    node->previous=NULL;
    node->next = queue->tail;
    queue->tail =node;
    
    return OK;
    
    
}

int insertAtQueue(Queue* queue,uint32_t nodeId){
    
    if(queue==NULL){
        printf("Queue is not allocate \n");
        error_Queue = NOT_ALLOCATED_QUEUE ;
        return NOT_ALLOCATED_QUEUE;
    }
    
    Node* node = createNode(nodeId);
    
    if(node==NULL){
        printf("No node given to insert.\n");
        error_Queue = NODE_NOT_GIVEN ;
        return NODE_NOT_GIVEN;
    }
    
    if(queue->tail==NULL){
        queue->tail = node;
        queue->head = node;
        return OK;
    }
    
    queue->tail->previous = node;
    node->previous=NULL;
    node->next = queue->tail;
    queue->tail =node;
    
    return OK;
    
}


