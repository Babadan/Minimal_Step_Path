#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "GStack.h"


int destroyGStack(GStack* stack){
    if(stack==NULL)
        return 0;
    
    GStackNode* temp ;
    while(stack->top!=NULL){
        temp=stack->top;
        stack->top = temp->previous;
        free(temp->value);
        free(temp);
    }
    free(stack);
    return 0;
}

bool gStackIsEmpty(GStack* stack){   
    if(stack->top==NULL)
        return true;
    return false;
}

GStack* createGStack(){
   
    GStack* s = malloc(sizeof(GStack));
    if(s ==NULL){
        printf("Fail to allocate Stack.\n");
        return NULL;
    }
    s->top=NULL;
  
    
    return s;
}

GStackNode* createGStackNode(void* value){
    
    GStackNode* stackNode = malloc(1*sizeof(GStackNode));
    if(stackNode ==NULL){
        printf("Fail to allocate StackNode. \n");
        return NULL;
    }
    
    stackNode->previous=NULL;
    stackNode->value =value;
    
    return stackNode;
}


GStackNode* popGStackNode(GStack* stack){
    //int nodeId;
    if(stack==NULL){
        printf("Stack not allocated.\n");
        return NULL;
    }
    if(stack->top==NULL){
        printf("Stack is empty.\n");
        return NULL;
    }
    
    GStackNode* sn;
    sn=stack->top;
    stack->top=sn->previous;
    
    //if(stack->top!=NULL)
        //stack->top->next=NULL;
    
    return sn;
    
}


int pushGStackNode(GStack* stack,GStackNode* stackNode){
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