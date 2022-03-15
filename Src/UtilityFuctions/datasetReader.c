#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "DatasetReader.h"



long int myfread(FILE* fileGraph ,void* buff){
    long int nread = 0, n=0;
    char* buffer = (char*)buff;
    
    nread = fread((char*) buff,ELEMSIZE ,CHUNK,fileGraph);
    
    if (feof(fileGraph) && nread==CHUNK ){
        memset(&buffer[nread-1],'\0',1);
    }
    
    if(nread==0){
        return EOF;
    }
    else if(nread != CHUNK){
        memset(&buffer[nread-1],'\0',1);
        return strlen(buff);
    }
    else{
        n = nread-1;
        
        while(strncmp(&buffer[n],"\n",1*sizeof(char))!=0){
            n--;
        }
        if(strncmp(&(buffer[n-1]),"S",1*sizeof(char)) == 0){
            memset(&buffer[n],'\0',1);
        }
  
        memset(&buffer[n+1],'\0',1);
        if(fseek(fileGraph, (n+1) - nread, SEEK_CUR)!=0){
            perror("Something went wrong with fseek.\n");
        }
    }
    
    
        
    return n+1;
}

long int countNewLine(void* buffer , long int size){
    char* buff = (char*)buffer;
    long int newLineCount = 0;
    long int i;
    
    if(buffer==NULL){
        printf("Buffer is NULL cannot count newlines");
        return -1;
    }
    
    char test[30];
    memcpy(test,buff,30);
    for(i=0; i<size; i++){
        if(buff[i]=='\n') newLineCount++;
    }
    return newLineCount;
}

uint32_t** graphChunkIntoArray(void* buff,long int size){
    
    uint32_t** graphArray;
    long int i,counter;
    long int cNewLine = countNewLine(buff,size);
    char* buffer = (char*) buff;
    char nodeId[10] ;
    int colum,row,flag;
    
    for(i=0; i<10; i++){
        nodeId[i]='\0';
    }
    
    if(cNewLine <= 0) return NULL;
    
    graphArray = malloc(cNewLine * sizeof(uint32_t*));
    
    for(i = 0; i<cNewLine; i++){
        graphArray[i]=malloc(2*sizeof(uint32_t)); 
    }
    
    
    counter=0;
    colum=0;
    row=0;
    flag=0;
    
    for(i = 0; i<size; i++){
        if(buffer[i] >='0' && buffer[i]<='9' ){
            nodeId[counter]=buffer[i];
            counter++;
            flag=1;
        }
        else if(flag==1){
            if(colum==0){
                if(nodeId[0]!='\0')
                    graphArray[row][0]=atoi(nodeId);
                memset(nodeId,'\0',10);
                colum=1;
            }
            else{
                if(nodeId[0]!='\0')
                    graphArray[row][1]=atoi(nodeId);
                memset(nodeId,'\0',10);
                colum=0;
                row++;
            }
            flag=0;
            counter=0;
        }
    }
    
    return graphArray;
    
   
}

int destroyGraphArray(uint32_t** graphArray,long int lines ){
    long int i ;
    for(i=0; i<lines; i++){
        free(graphArray[i]);
    }
    free(graphArray);
    return 0;
} 
    




