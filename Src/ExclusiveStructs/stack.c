#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Stack.h"

int destroyStack(Stack* stack){
    if(stack==NULL)
        return 0;
    
    StackNode* temp ;
    while(stack->top!=NULL){
        temp=stack->top;
        stack->top = temp->previous;
        free(temp);
    }
    free(stack);
    return 0;
}

bool stackIsEmpty(Stack* stack){
    
    if(stack->top==NULL)
        return true;
    return false;
}

Stack* createStack(){
   
    Stack* s = malloc(sizeof(Stack));
    if(s ==NULL){
        printf("Fail to allocate Stack.\n");
        return NULL;
    }
    s->top=NULL;
  
    
    return s;
}

StackNode* createStackNode(uint32_t nodeId){
    
    StackNode* stackNode = malloc(1*sizeof(StackNode));
    if(stackNode ==NULL){
        printf("Fail to allocate StackNode. \n");
        return NULL;
    }
    
    stackNode->previous=NULL;
    //stackNode->next=NULL;
    stackNode->index=0;
    stackNode->lowLink=0;
    stackNode->nodeId = nodeId;
    stackNode->currentNeightbor=-1;
    
    return stackNode;
}

StackNode* popStackNode(Stack* stack){
    //int nodeId;
    if(stack==NULL){
        printf("Stack not allocated.\n");
        return NULL;
    }
    if(stack->top==NULL){
        printf("Stack is empty.\n");
        return NULL;
    }
    
    StackNode* sn;
    sn=stack->top;
    stack->top=sn->previous;
    
    //if(stack->top!=NULL)
        //stack->top->next=NULL;
    
    return sn;
    
}

int pushStackNode(Stack* stack,StackNode* stackNode){
    if(stack==NULL){
        printf("Stack not allocated.\n");
        return -1;
    }
    if(stackNode==NULL){
        printf("StackNode is NULL.\n");
        return -1;
    }
    stackNode->previous=stack->top;
    stack->top=stackNode;
    //stackNode->next=NULL;
    return 1;
}

