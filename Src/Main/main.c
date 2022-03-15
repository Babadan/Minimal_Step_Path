#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DatasetReader.h"
#include "Graph.h"
#include "GraphQA.h"
#include "CompareResults.h"
#include "WeaklyConectedComponents.h"
#include "StronglyConnectedComponents.h"
#include "HyperGraph.h"
#include "Grail.h"


#define ELEMSIZE 1


DoubleGraph dG;

int main(int argc, char** argv) {
    
    FILE* fileGraph;
    FILE* workFile;
    FILE* resultFile;
    FILE* correctFile;
    char buffer[10];    
    
    
    printf("Enter graph size to load.\n");
    scanf("%s",buffer);
    //strncpy(buffer,"medium",6);
    
    //printf("Enter execution type \"parallel\",\"serial\",\"serialNoIndex\".\n ");
    
    
    if(strncmp(buffer,"tiny",4)==0){
        fileGraph = fopen("./Datasets/tiny/tinyGraph.txt","r");
    }
    else if(strncmp(buffer,"small",5)==0){
        fileGraph = fopen("./Datasets/small/smallGraph.txt","r");
    }
    else if(strncmp(buffer,"medium",6)==0){
        fileGraph = fopen("./Datasets/medium/mediumGraph.txt","r");
    }
    else if(strncmp(buffer,"large",5)==0){
        fileGraph = fopen("./Datasets/large/largeGraph.txt","r");
    }
    else{
        printf("Error : Enter tiny , small , medium or large.\n");
        return -1;
    }
    
          
    dG = graphCreate(fileGraph);
    
    if(strncmp(buffer,"tiny",4)==0){
        workFile = fopen("./Datasets/tiny/tinyWorkload_FINAL.txt","r");
        correctFile = fopen("./Datasets/tiny/tinyWorkload_RESULTS.txt","r");
        resultFile = fopen("./Results/tiny/ResultstinyGraph.txt","w+");
    }
    else if(strncmp(buffer,"small",5)==0){
        workFile = fopen("./Datasets/small/smallWorkload_FINAL.txt","r");
        correctFile = fopen("./Datasets/small/smallWorkload_RESULTS.txt","r");
        resultFile = fopen("./Results/small/ResultssmallGraph.txt","w+");
    }
    else if(strncmp(buffer,"medium",6)==0){
        workFile = fopen("./Datasets/medium/mediumWorkload_FINAL.txt","r");
        correctFile = fopen("./Datasets/medium/mediumWorkload_RESULTS.txt","r");
        resultFile = fopen("./Results/medium/ResultsmediumGraph.txt","w+");
    }
    else if(strncmp(buffer,"large",5)==0){
        workFile = fopen("./Datasets/large/largeWorkload_FINAL.txt", "r");
        correctFile = fopen("./Datasets/large/largeWorkload_RESULTS.txt", "r");
        resultFile = fopen("./Results/large/ResultslargeGraph.txt","w+");
    }
    else{
        printf("Error : Enter tiny , small , medium or large.\n");
        return -1;
    }
    
    
    //executeWorkLoadSerially(workFile,resultFile,&dG);
    //executeWorkLoadSeiriallyNoIndex(workFile,resultFile,dG);
    executeWorkLoadParallel(workFile,resultFile);
    
    
    if (fclose(resultFile)== EOF){
        perror("Something went wrong with file closing.");
    }
    
   
    if(strncmp(buffer,"tiny",4)==0){
        resultFile = fopen("./Results/tiny/ResultstinyGraph.txt","r");
    }
    else if(strncmp(buffer,"small",5)==0){
        resultFile = fopen("./Results/small/ResultssmallGraph.txt","r");
    }
    else if(strncmp(buffer,"medium",6)==0){
        resultFile = fopen("./Results/medium/ResultsmediumGraph.txt","r");
    }
    else if(strncmp(buffer,"large",5)==0){
        resultFile = fopen("./Results/large/ResultslargeGraph.txt","r");
    }
    else{
        printf("Error : Enter tiny , small , medium or large.\n");
        return -1;
    }
    
    

    destroyGraph(dG.graphIngoing);
    destroyGraph(dG.graphOutgoing);
    
    
    compareResults(correctFile,resultFile);
    
    if (fclose(workFile)== EOF){
        perror("Something went wrong with file closing.");
    }
    
    if (fclose(resultFile)== EOF){
        perror("Something went wrong with file closing.");
    }
   
    if (fclose(correctFile)== EOF){
        perror("Something went wrong with file closing.");
    }
   
    if (fclose(fileGraph)== EOF){
        perror("Something went wrong with file closing.");
    }
    

    return (EXIT_SUCCESS);
}

