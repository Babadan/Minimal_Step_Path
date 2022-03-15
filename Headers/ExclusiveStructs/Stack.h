#ifndef STACK_H
#define STACK_H
#include <stdint.h>

typedef struct StackNode{
    uint32_t nodeId;
    uint32_t index;
    uint32_t lowLink;
    int currentNeightbor;
    
    struct StackNode* previous;
    
    
}StackNode;

typedef struct Stack{
    StackNode* top;
}Stack;


//int pushStack(Stack* stack,uint32_t nodeId);
StackNode* popStackNode(Stack* stack);
//int popStack(Stack* stack);
int pushStackNode(Stack* stack,StackNode* stackNode);

StackNode* createStackNode(uint32_t nodeId);
Stack* createStack();
bool stackIsEmpty(Stack* stack);
int destroyStack(Stack* stack);
int removeFromStack(Stack* stack, StackNode* stackNode);

#endif /* STACK_H */

