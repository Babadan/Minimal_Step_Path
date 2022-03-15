/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DatasetReader.h
 * Author: lumberjack
 *
 * Created on October 26, 2016, 7:09 PM
 */
#ifndef DATASETREADER_H
#define DATASETREADER_H

#include<stdint.h>

#define CHUNK 100  // Chunk size must be at least as the longest line in characters  

#define ELEMSIZE 1


long int myfread(FILE* fileGraph ,void* buff);

long int countNewLine(void* buffer , long int size);
uint32_t** graphChunkIntoArray(void* buff,long int size);
int destroyGraphArray(uint32_t** graphArray,long int lines );



#endif /* DATASETREADER_H */

