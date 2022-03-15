#ifndef GQUEUE_H
#define GQUEUE_H

#include <stdint.h>

typedef struct GQueueNode{
    
    void* value;
    struct GQueueNode* next;
    struct GQueueNode* previous;
    
    
}GQueueNode;

typedef struct GQueue{
    
    GQueueNode* head;
    GQueueNode* tail;
    uint32_t size;
    
}GQueue;

GQueue* createGQueue();
GQueueNode* createGQueueNode();
int insertAtGQueue(GQueue* queue ,GQueueNode* queueNode);
GQueueNode* takefromGQueue(GQueue* queue);
int destroyGqueue(GQueue* queue);
int destroyGQueueNode(GQueueNode* queueNode);

#endif /* GQUEUE_H */

