#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DatasetReader.h"
#include "Graph.h"
#include "GraphQA.h"
#include "CompareResults.h"

int compareResults(FILE* correctResultsFile,FILE* produsedResultsFile ){
    
    char* correctRes = malloc(5*sizeof(char));
    char* producedRes = malloc(5*sizeof(char));
  
    long int* mistakeLine = malloc(sizeof(long int)*MISTAKE_TABLE_SIZE);
    long int mistakeLineSize = MISTAKE_TABLE_SIZE;
    long int i;
    long int mistakesCount=0;
    long int linecounter=1;
    
    
    correctRes = fgets(correctRes,5,correctResultsFile);
    producedRes = fgets(producedRes,5,produsedResultsFile);
        
    while(correctRes!=NULL && producedRes!=NULL){
                   
                    
        if(strcmp(correctRes,producedRes)!=0){
            printf("%s -- %s ",correctRes,producedRes);            
            mistakeLine[mistakesCount]=linecounter;
            mistakesCount++;
            if(mistakesCount == mistakeLineSize){
                mistakeLineSize*=2;
                mistakeLine= realloc(mistakeLine,mistakeLineSize*sizeof(long int));
            }
        }
        
        linecounter++;
        correctRes = fgets(correctRes,5,correctResultsFile);
        producedRes = fgets(producedRes,5,produsedResultsFile);
    }
    
    if(mistakesCount!=0){
        for(i=0; i<mistakesCount; i++){
            printf("Error found comparing workload results on line : (%ld)\n ",mistakeLine[i]);
        }
        free(mistakeLine);
        free(producedRes);
        free(correctRes);
        return FOUND_MISTAKES ;
    }
    
    printf("Congratulations no error found . :-)\n");
    free(mistakeLine);
    free(producedRes);
    free(correctRes);
    
    return NO_MISTAKES ;
    
}