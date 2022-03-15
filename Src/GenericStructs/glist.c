#include <stdio.h>
#include <stdlib.h>
#include "GList.h"

List* createList(){
    List* list = malloc(1*sizeof(List));
    list->head=NULL;
    list->tail=NULL;
    list->size=0;
    return list;
    
}

ListNode* createListNode(){
    ListNode* listNode = malloc(1*sizeof(ListNode));
    listNode->value=NULL;
    listNode->next =NULL;
    listNode->previous=NULL;
    return listNode;
}

int insertAtList(List* list ,ListNode* listNode){
    
    if(list ==NULL){
        printf("List not allocated.\n");
        return -1;
    }
    if(listNode==NULL){
        printf("List node not allocated.\n");
        return -1;
    }
    
    ListNode* ls;
    
    if(list->head==NULL){
        list->head=listNode;
        list->tail=listNode;
    }
    else{
        ls=list->head;
        list->head->next=listNode;
        list->head=listNode;
        listNode->previous=ls;
    }
    list->size++;
    return 1;
    
    
}

ListNode* takefromListEnd(List* list){
    
    if(list ==NULL){
        printf("List not allocated.\n");
        return NULL;
    }
    if(list->tail ==NULL){
        printf("List is empty.\n");
        return NULL;
    }
    ListNode* ls =list->tail;
    if(ls->next==NULL){
        list->head=NULL;
        list->tail=NULL;
    }
    else{
        
        list->tail = ls->next;
        ls->next->previous=NULL;
       
    }
    list->size--;
    return ls;
    
    
}

int destroyList(List* list){
    
    if(list==NULL)
        return 0;
    
    ListNode* temp ;
    while(list->head!=NULL){
        temp=list->head;
        list->head = temp->previous;
        if(temp->value!=NULL){
            free(temp->value);
        }
        free(temp);
    }
    free(list);
    return 0;
    
    
}

int destroyListNode(ListNode* listNode){
    
    if(listNode==NULL){
        printf("ListNodeNotAllocated.\n");
        return -1;
    }
    
    free(listNode->value);
    free(listNode);
    return 0;
}