#ifndef QUEUE_H
#define QUEUE_H

#define OK 0
#define FAIL_MEM_ALLOCATION -1
#define FAIL_DEALLOCATING_QUEUE -2
#define NODE_NOT_GIVEN -3
#define NOT_ALLOCATED_QUEUE -4
#define EMPTY_QUEUE -5


typedef struct Node{
    uint32_t node; 
    struct Node* next;
    struct Node* previous;
}Node;

typedef struct Queue{
    Node* head;
    Node* tail;
}Queue;

extern int error_Queue;


Queue* createQueue();
int initQueue(Queue* queue);
void errorQueue(char* errString);
Node* takeNode(Queue* queue);
Node* createNode(uint32_t node);
int insertAtQueue(Queue* queue,uint32_t nodeId);
bool isEmpty(Queue* queue);
int destroyQueue(Queue* queue);
int insertNodeAtQueue(Queue* queue,Node* node);
int deleteNodeFromQueue(Queue* queue,Node* node);
int moveNodeToEnd(Queue* queue,Node* node);
Node* takeFromTail(Queue* queue );
Node* lookAtTail(Queue* queue);
Node* looknNode(Queue* queue,int possition);

#endif /* QUEUE_H */

