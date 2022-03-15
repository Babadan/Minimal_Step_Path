/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   NodeIndex1.h
 * Author: lumberjack
 *
 * Created on November 2, 2016, 8:25 AM
 */

#ifndef NODEINDEX1_H
#define NODEINDEX1_H

#define INDEX_START_SIZE 100

#define OK 0
#define FAIL_MEM_ALLOCATION -1
#define FAIL_DEALLOCATING_NODELIST -2
#define BUFFER_NOT_ALLOCATED -3
#define NODEINDEX_NOT_ALLOCATE -4
#define FAIL_TO_ALLOCATE_NODEINDEXNODE -5
#define INDEX_TABLE_NOT_ALLOCATED -6
#define GRAPHNODE_NOT_FOUND -7

#define RESIZE_PERCENTAGE 20

#define HASHSTARTSIZE 4
#define BUCKETSIZE 6

#include "GHash.h"
extern int nodeList_error; 

typedef struct Index {
    
    long int* bufferoffset;
    long int tail;
    HashTable* hashTable;
    
} Index;

typedef struct NodeIndex{
    Index* indexTable;
    uint32_t size;
} NodeIndex ;

NodeIndex* createNodeIndex();
int insertNode(NodeIndex* node ,uint32_t nodeId );
long int getListHead(NodeIndex* node ,uint32_t nodeId);
int destroyNodeIndex(NodeIndex* node);

Index* getNodeIndex(NodeIndex* nodeIndex,uint32_t nodeId);

bool checkNodeExists(NodeIndex* nodeIndex, uint32_t nodeId);
int check_resize(NodeIndex*,uint32_t);

list_node* getListTail(Index index,char* bufferStart);

#endif /* NODEINDEX1_H */

