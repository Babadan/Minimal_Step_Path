#ifndef GSTACK_H
#define GSTACK_H

typedef struct GStackNode{
    void* value;
    struct GStackNode* previous;
}GStackNode;

typedef struct GStack{
    GStackNode* top;
}GStack;


bool gStackIsEmpty(GStack* stack);
GStack* createGStack();
GStackNode* createGStackNode(void* value);
GStackNode* popGStackNode(GStack* stack);
int pushGStackNode(GStack* stack,GStackNode* stackNode);
int destroyGStack(GStack* stack);

#endif /* GSTACK_H */

