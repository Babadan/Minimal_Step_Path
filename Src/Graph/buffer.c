#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "Buffer.h"
#include <assert.h>
#include <stdbool.h>

int buffer_error = OK; 

Buffer* createBuffer(){
    Buffer* buff;
    long int offset = METADATA_SIZE;
    long int bufferSize = STARTBUFFERSIZE;
    
    buff = malloc(1*sizeof(Buffer));
    
    if(buff == NULL){
        printf("Something went wrong with malloc. %s \n ",strerror(errno));
        buffer_error = FAIL_MEM_ALLOCATION;
        return NULL;
    }
    
    buff->buffer = calloc(sizeof(char),STARTBUFFERSIZE);
    
    if(buff->buffer == NULL){
        printf("Something went wrong with malloc. %s \n ",strerror(errno));
        buffer_error = FAIL_MEM_ALLOCATION;
        return NULL;
    }
    
    memcpy(&(buff->buffer[FILL_INDICATOR]),&offset,sizeof(long int));
    memcpy(&(buff->buffer[BUFFER_SIZE_OFFSET]),&bufferSize,sizeof(long int));
    
    return buff;
}

char* allocateNewNode(Buffer* buff){
    long int offset;
    long int bufferSize;
    char* ptr ;
    
    memcpy(&offset,&(buff->buffer[FILL_INDICATOR]),sizeof(long int));
    memcpy(&bufferSize,&(buff->buffer[BUFFER_SIZE_OFFSET]),sizeof(long int));
            
    ptr=buff->buffer+offset;
    offset+=sizeof(list_node);
    memcpy(buff->buffer,&offset,sizeof(long int));
    
    return ptr;
}

bool testBufferCapacity(Buffer* buff,uint32_t insertionSize){
    
    long int offset;
    long int bufferSize;
    
    memcpy(&offset,&(buff->buffer[FILL_INDICATOR]),sizeof(long int));
    memcpy(&bufferSize,&(buff->buffer[BUFFER_SIZE_OFFSET]),sizeof(long int));
    
    if((offset-5+insertionSize)<=bufferSize){
        return true;
    }
    else{
        return false;
    }
}

list_node* getListNode(char* ptr){
    return (list_node*)ptr;   
}

int destroyBuffer(Buffer* buff){
    
    if(buff == NULL){
        buffer_error = FAIL_DEALLOCATING_BUFFER;
        return -1;
    }
    
    if(buff->buffer == NULL){
        free(buff);
        return OK;
    }
    else{
        free(buff->buffer);
        free(buff);
        return OK;
    }
}

long int getOffset(Buffer* buff){
    long int offset;
    memcpy(&offset,&(buff->buffer[FILL_INDICATOR]),sizeof(long int));
    return offset;
}

void setOffset(Buffer* buff, long int offset){
    memcpy(&(buff->buffer[FILL_INDICATOR]),&offset,sizeof(long int));
}

Buffer* resizeBuffer(Buffer* buff){
    
    long int bufferSize;
    long int offset;
    char* newBuffer;
    long int counter,i;
    list_node* runner;
    
    assert(buff!= NULL);
    
    memcpy(&bufferSize,&(buff->buffer[BUFFER_SIZE_OFFSET]),sizeof(long int));
    memcpy(&offset,&(buff->buffer[FILL_INDICATOR]),sizeof(long int));
    bufferSize*=2;
    memcpy(&(buff->buffer[BUFFER_SIZE_OFFSET]),&bufferSize,sizeof(long int));
    
   
    newBuffer=calloc(sizeof(char),bufferSize);
    if(newBuffer==NULL){
        printf("Something went wrong with malloc. %s \n ",strerror(errno));
        buffer_error = FAIL_MEM_ALLOCATION;
        return NULL;
    }
//---------------------------------Reinitiallize list_node pointer of the old buff to the new one-----------------------------     
    i=0;
    runner = (list_node*)(buff->buffer+METADATA_SIZE);
    counter = METADATA_SIZE;
    while(counter+sizeof(list_node)<=offset){
        if(runner[i].nextListNode!=NULL){
            runner[i].nextListNode = newBuffer+(runner[i].nextListNode - buff->buffer);
        }
        counter+=sizeof(list_node);
        i++;
    }
    
//----------------------------------------------------------------------------------------------------------------------------    
    memcpy(newBuffer,(buff->buffer),bufferSize/2);
    
   
    free((buff->buffer));
    buff->buffer=newBuffer;
    
    return buff;
    
    
    
}

void BufferError(char* errString){
    printf("%s: ", errString);
    
    switch (buffer_error) {
        case OK:
            perror("EVERYTHING IS OK");
            break;
        case FAIL_MEM_ALLOCATION:
            perror("FAILD MEMORY ALLOCATION");
            break;
    }
}
