#ifndef BUFFER_H
#define BUFFER_H
#include<stdint.h>
#include<stdbool.h>

#define NEIGHBOR_SIZE_ARRAY 6
#define OK_SUCCESS 0
#define FAILURE 1
#define STARTBUFFERSIZE 100

#define FILL_INDICATOR 0 //At the start of the buffer there is an offset that points to the last ellemnt of the buffer  
#define BUFFER_SIZE_OFFSET FILL_INDICATOR + sizeof(long int)
#define METADATA_SIZE 32

#define OK 0
#define FAIL_MEM_ALLOCATION -1
#define FAIL_DEALLOCATING_BUFFER -2

#define OUTGOING -1
#define INCOMING 1

extern int buffer_error ; 

typedef struct list_node{
    uint32_t neighbor[NEIGHBOR_SIZE_ARRAY];
    
    uint32_t edgeProperty[NEIGHBOR_SIZE_ARRAY];
    
    uint32_t fillFactor;
    
    char* nextListNode;

}list_node;

typedef struct Buffer {
    char* buffer;

}Buffer;

Buffer*  createBuffer();
char* allocateNewNode(Buffer*);
struct list_node* getListNode(char*);
int destroyBuffer(Buffer*);
Buffer* resizeBuffer(Buffer* buff);
//list_node* getListTail(list_node* listNode);
bool testBufferCapacity(Buffer* buff,uint32_t insertionSize);
long int getOffset(Buffer* buff);
void setOffset(Buffer* buff, long int offset);

#endif /* BUFFER_H */

