
#ifndef LIST_H
#define LIST_H

#include <stdint.h>
typedef struct ListNode{
    
    void* value;
    struct ListNode* next;
    struct ListNode* previous;
    
    
}ListNode;

typedef struct List{
    
    ListNode* head;
    ListNode* tail;
    uint32_t size;
    
}List;


List* createList();
int insertAtList(List* list,ListNode* listNode);
ListNode* takefromListEnd(List* list);
int destroyList(List* list);
int destroyListNode(ListNode* listNode);
ListNode* createListNode();




#endif /* LIST_H */

