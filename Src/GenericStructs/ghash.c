#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "GHash.h"



int hashFunction(Entry *entry, HashTable *hashTable){
    if(entry->key != 0)
        return entry->key % hashTable->index_size;
    return 0;
}

//Knuths multiplicive Hash.
uint32_t hashFunction2(uint32_t a,HashTable *hashTable){
   a = a * UINT32_C(2654435761);
   return a % hashTable->index_size;
}

//uint32_t hashFunction3(char* s,HashTable *hashTable){ // Dan Bernstein hash function
//    unsigned long h = 5381;
//    int c;
//    while((c=*s++))
//        h = ((h<< 5) + h) +c;
//        
//    return h % hashTable->index_size;
//}

#define FNV_32_PRIME 16777619

uint32_t hval = 0;
uint32_t hashFunction3(char* s,HashTable *hashTable){ // Dan Bernstein hash function
    unsigned char *bp = (unsigned char*)s;
    unsigned char *be = bp + 24;
    while(bp<be){
#if defined(NO_FNV_GCC_OPTIMIZATION)
        hval *= FNV_32_PRIME;
#else
        hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24); 
#endif
        hval ^= (uint32_t) *bp++;
    }
    return hval % hashTable->index_size;
}

//uint32_t hval2 = 0;
//uint32_t hashFunction2(uint32_t key ,HashTable *hashTable){ // Dan Bernstein hash function   
//    
//    char s[24] ;
//    snprintf(s,24,"%u",key);
//    
//    memset(s+1,'\0',23);
//    
//    unsigned char *bp = (unsigned char*)s;
//    unsigned char *be = bp + 24;
//    
//   
//    while(bp<be){
//#if defined(NO_FNV_GCC_OPTIMIZATION)
//        hval *= FNV_32_PRIME;
//#else
//        hval2 += (hval2<<1) + (hval2<<4) + (hval2<<7) + (hval2<<8) + (hval2<<24); 
//#endif
//        hval2 ^= (uint32_t) *bp++;
//    }
//    
//    return hval2 % hashTable->index_size;
//}


////////////////////////////////////////////////////////////
void initHashTable(HashTable *hashTable, int init_size, int bucket_max_size){  
  int i;
  if(hashTable == NULL)
    printf("initHashTable :: hashTable == NULL");
  else{
    hashTable->index = (IndexCell *)malloc(sizeof(IndexCell)*init_size);
    hashTable->index_size = init_size;
    hashTable->depth = log(init_size)/log(2);
    hashTable->bucketSize=bucket_max_size;
    initIndexCell(&(hashTable->index[0]), bucket_max_size, 0, 0); //create first bucket 
    for(i = 1; i < init_size; i++)  
      initIndexCell2(&(hashTable->index[i]), &(hashTable->index[0]));
  }
}

void initHashTable2(HashTable *hashTable, int init_size, int bucket_max_size){  
  int i;
  if(hashTable == NULL)
    printf("initHashTable :: hashTable == NULL");
  else{
    hashTable->index = (IndexCell *)malloc(sizeof(IndexCell)*init_size);
    hashTable->index_size = init_size;
    hashTable->depth = log(init_size)/log(2);
    hashTable->bucketSize=bucket_max_size;
    for(i = 0; i < init_size; i++)  
      initIndexCell(&(hashTable->index[i]), bucket_max_size, hashTable->depth, i); // every cell has a bucket 
  }
}

int addToHashTable(HashTable *hashTable, Entry *entry){
    int i,j;
    int index_i;
    int countEntries = -1;

    if(hashTable == NULL || hashTable->index == NULL || hashTable->index[0].bucket == NULL){
      printf("HashTable not created properly.\n ");
      return -1 ;
    }

    int bucketSize = hashTable->bucketSize;

    Entry** entryTable = calloc(bucketSize+2,sizeof(Entry*));

    entryTable[0] = entry;
    countEntries++;
    
    if(hashTable == NULL){
        printf("addToHashTable :: hashTable == NULL");
        return -1;
    }
 
    while(countEntries!=-1){
           
        index_i = hashFunction2(entryTable[countEntries]->key, hashTable);
        
        
        if(!fullBucket(hashTable->index[index_i].bucket,hashTable->bucketSize)){
            addToIndexCell(&(hashTable->index[index_i]), entryTable[countEntries]);
            countEntries--;
        }
        else{
            if(hashTable->index[index_i].bucket->depth < hashTable->depth){
                //Split buckets only - increase bucket depth

                Bucket *old_bucket = hashTable->index[index_i].bucket;
                int max_size = hashTable->bucketSize;

                spiltBucket(hashTable,old_bucket);

                //ReInsert old entries to new buckets
                j=0;
                for(i = (countEntries+1); i <= max_size; i++){
                    entryTable[i] = old_bucket->entries[j];
                    j++;
                }
                countEntries = j;
                entryTable[i] = entry;
                free(old_bucket->entries);
                free(old_bucket);
            }
            else{
                expandHash(hashTable);
            }
        } 
  
    }
    
    free(entryTable);
    return 1;
}

int addToHashTableStringKey(HashTable *hashTable, Entry *entry){
    int i,j;
    int index_i;
    int countEntries = -1;

    if(hashTable == NULL || hashTable->index == NULL || hashTable->index[0].bucket == NULL){
      printf("HashTable not created properly.\n ");
      return -1 ;
    }

    int bucketSize = hashTable->bucketSize;

    Entry** entryTable = calloc(bucketSize+2,sizeof(Entry*));

    entryTable[0] = entry;
    countEntries++;
    
    if(hashTable == NULL){
        printf("addToHashTable :: hashTable == NULL");
        return -1;
    }
 
    while(countEntries!=-1){
           
        index_i = hashFunction3((char*)entryTable[countEntries]->pointer, hashTable);
        
        
        if(!fullBucket(hashTable->index[index_i].bucket,hashTable->bucketSize)){
            addToIndexCell(&(hashTable->index[index_i]), entryTable[countEntries]);
            countEntries--;
        }
        else{
            if(hashTable->index[index_i].bucket->depth < hashTable->depth){
                //Split buckets only - increase bucket depth

                Bucket *old_bucket = hashTable->index[index_i].bucket;
                int max_size = hashTable->bucketSize;

                spiltBucket(hashTable,old_bucket);

                //ReInsert old entries to new buckets
                j=0;
                for(i = (countEntries+1); i <= max_size; i++){
                    entryTable[i] = old_bucket->entries[j];
                    j++;
                }
                countEntries = j;
                entryTable[i] = entry;
                free(old_bucket->entries);
                free(old_bucket);
            }
            else{
                expandHash(hashTable);
            }
        } 
  
    }
    
    free(entryTable);
    return 1;
}


int destroyHashEntriesValues(HashTable *hashTable){
    int i,j,k;
    int bucketDepth;
    int valueSizeMap = 10;
    
    bool* map = malloc(hashTable->index_size*sizeof(bool));
    void** valueMap = malloc(valueSizeMap * sizeof(void*)); 
    
    for(i=0; i<hashTable->index_size; i++){
        map[i]=true;
    }
    
    for(i=0; i<hashTable->index_size; i++){
        bucketDepth=hashTable->index[i].bucket->depth ;
        if(map[i]){
            j=i;
            while(j<hashTable->index_size){
                map[j]=false;
                j+=pow(2,bucketDepth); 
            }
            for(k=0; k<hashTable->bucketSize; k++){
                if(hashTable->index[i].bucket->entries[k]==NULL)
                    break;
                
//                flag = true;
//                for(n=0; n<valueMapCounter; n++){
//                    if(valueMap[n]==hashTable->index[i].bucket->entries[k]->pointer){
//                        flag = false;
//                    }
//                }
//                
//                if(flag){
//                    free(hashTable->index[i].bucket->entries[k]->pointer);
//                    valueMap[valueMapCounter] = hashTable->index[i].bucket->entries[k]->pointer;
//                    valueMapCounter++;
//                    if(valueMapCounter==valueSizeMap){
//                        valueSizeMap +=  (0.1 * valueSizeMap + 1);
//                        valueMap = realloc(valueMap,valueSizeMap*sizeof(void*));
//                    }
//                }
                free(hashTable->index[i].bucket->entries[k]->pointer);
                free(hashTable->index[i].bucket->entries[k]);
            }
            free(hashTable->index[i].bucket->entries);
        }      
    }
    
    free(valueMap);
    
    for(i=0; i<hashTable->index_size; i++){
        map[i]=true;
    }
    
    for(i=0; i<hashTable->index_size; i++){
        
        if(map[i]){
            j=i;
            bucketDepth=hashTable->index[i].bucket->depth ;
            while(j<hashTable->index_size){
                map[j]=false;
                j+=pow(2,bucketDepth); 
            }
            free(hashTable->index[i].bucket);
        }
        
    }
    free(map);
    free(hashTable->index);
    return 1;
    //free(hashTable);    
}

int destroyHashEntries(HashTable *hashTable){
    int i,j,k;
    int bucketDepth;
    
    bool* map = malloc(hashTable->index_size*sizeof(bool));
    
    for(i=0; i<hashTable->index_size; i++){
        map[i]=true;
    }
    
   
    for(i=0; i<hashTable->index_size; i++){
        bucketDepth=hashTable->index[i].bucket->depth ;
        if(map[i]){
            j=i;
            while(j<hashTable->index_size){
                map[j]=false;
                j+=pow(2,bucketDepth); 
            }
            for(k=0; k<hashTable->bucketSize; k++){
                if(hashTable->index[i].bucket->entries[k]==NULL)
                    break;
                
                //free(hashTable->index[i].bucket->entries[k]->pointer);
                free(hashTable->index[i].bucket->entries[k]);
            }
            free(hashTable->index[i].bucket->entries);
        }      
    }
    
    for(i=0; i<hashTable->index_size; i++){
        map[i]=true;
    }
    
    for(i=0; i<hashTable->index_size; i++){
        
        if(map[i]){
            j=i;
            bucketDepth=hashTable->index[i].bucket->depth ;
            while(j<hashTable->index_size){
                map[j]=false;
                j+=pow(2,bucketDepth); 
            }
            free(hashTable->index[i].bucket);
        }
        
    }
    free(map);
    free(hashTable->index);
    return 1;   
}

int destroyHash(HashTable *hashTable){
    int i,j;
    int bucketDepth;
    
    if(hashTable->index_size==0){
        return 0;
    }
    bool* map = malloc(hashTable->index_size*sizeof(bool));
    
    for(i=0; i<hashTable->index_size; i++){
        map[i]=true;
    }
    
    for(i=0; i<hashTable->index_size; i++){
        bucketDepth=hashTable->index[i].bucket->depth ;
        if(map[i]){
            j=i;
            while(j<hashTable->index_size){
                map[j]=false;
                j+=pow(2,bucketDepth); 
            }
            free(hashTable->index[i].bucket->entries);
        }      
    }
    
    for(i=0; i<hashTable->index_size; i++){
        map[i]=true;
    }
    
    for(i=0; i<hashTable->index_size; i++){
        
        if(map[i]){
            j=i;
            bucketDepth=hashTable->index[i].bucket->depth ;
            while(j<hashTable->index_size){
                map[j]=false;
                j+=pow(2,bucketDepth); 
            }
            free(hashTable->index[i].bucket);
        }
        
    }
    free(map);
    free(hashTable->index);
    return 1;
}

void* getEntry(HashTable* hashTable, int key ){
    
    int indexI ;
    if(hashTable==NULL){
        printf("HashTable not allocated. \n");
        return NULL;
    }
    
    void* pointer;
    int i ;
    int k;
    
    indexI = hashFunction2(key,hashTable);
    
    if(hashTable->index==NULL){
        printf("IndexIn hash not allocated not allocated.\n ");
        return NULL;
    }
    
    int bucketSize = hashTable->bucketSize;
    
    for(i=0; i<bucketSize; i++){
        
        if(hashTable->index[indexI].bucket->entries==NULL)
            break;
        if(hashTable->index[indexI].bucket->curr_size==0)
            break;
        if(hashTable->index[indexI].bucket->entries[i]==NULL)
            break;
        
        k = hashTable->index[indexI].bucket->entries[i]->key;
        pointer = hashTable->index[indexI].bucket->entries[i]->pointer;
        if(k == -1){
            break;
        }
        if(key==k){
            return pointer; 
        }
        
    }
            
    return NULL;
}

int getEntryStringKey (HashTable* hashTable, char* key ){
    
    int indexI ;
    if(hashTable==NULL){
        printf("HashTable not allocated. \n");
        return -1;
    }
    
    
    int i ;
    char* k;
    
    indexI = hashFunction3(key,hashTable);
    
    if(hashTable->index==NULL){
        printf("IndexIn hash not allocated not allocated.\n ");
        return -1;
    }
    
    int bucketSize = hashTable->bucketSize;
    
    for(i=0; i<bucketSize; i++){
        
        if(hashTable->index[indexI].bucket->entries==NULL)
            break;
        if(hashTable->index[indexI].bucket->curr_size==0)
            break;
        if(hashTable->index[indexI].bucket->entries[i]==NULL)
            break;
        
        k = hashTable->index[indexI].bucket->entries[i]->pointer;
       
        
        if(strcmp(key,k)==0){
            return hashTable->index[indexI].bucket->entries[i]->key; 
        }
        
    }
            
    return -1;
}

HashTable* spiltBucket(HashTable *hashTable, Bucket* old_bucket){
    
        int first_of_pointers = old_bucket->first_of_pointers;
	int max_size = hashTable->bucketSize;
	int old_depth = old_bucket->depth;
	int new_depth = old_depth + 1;
	

	int first_index = first_of_pointers;
	
        int second_index = first_index + pow(2,old_depth);
        
        
        
	//Creating the new buckets
	initIndexCell(&(hashTable->index[first_index]), max_size, new_depth, first_index);
	initIndexCell(&(hashTable->index[second_index]), max_size, new_depth, second_index);
	//Fixing pointers to new buckets
       
        int counter = first_index+1;
	//for(i = 1; i < new_pointers_no; i++ ){
        
        while(counter<hashTable->index_size){ 
            initIndexCell2(&(hashTable->index[counter-1]), &(hashTable->index[first_index]));
            counter+= (pow(2,old_depth));
            initIndexCell2(&(hashTable->index[counter-1]), &(hashTable->index[second_index]));
            counter+= (pow(2,old_depth));
	}
        return hashTable;
    
}

HashTable* expandHash(HashTable *hashTable){
    //Index -> DoubleSize - increase index depth
	//printf("Doubling Index\n");
	//printf("Depth is : %d \n",hashTable->depth);
        
	IndexCell *old_index = hashTable->index;
        int i;
	int old_size = hashTable->index_size;
	int old_depth = hashTable->depth;
	int new_index_size = old_size*2;
	int new_depth = old_depth + 1;
	hashTable->index = (IndexCell *)malloc(sizeof(IndexCell)*new_index_size);
	//Initialize new Index
	for(i = 0; i < old_size; i++){
	  initIndexCell2(&(hashTable->index[i]), &(old_index[i]));
	  initIndexCell2(&(hashTable->index[i + old_size]), &(old_index[i]));
	}
	hashTable->index_size = new_index_size;
	hashTable->depth = new_depth;
	free(old_index);
        return hashTable;
    
}

Entry* createEntry(int key, void* value){
    Entry* entry = malloc(1*sizeof(Entry));
    
    entry->key = key;
    entry->pointer = value;
    return entry;
}

void printHashTable(HashTable *hashTable){
  int i;
  if(hashTable == NULL)
    printf("printHashTable :: hashTable == NULL");
  else{
    printf("printing a hashTable...\n");
    for(i = 0; i < hashTable->index_size; i++){
      printIndexCell(&(hashTable->index[i]));
      printf("\n");
    }
  }
}

////////////////////////////////////////////////////////////
void initIndexCell(IndexCell *indexCell, int max_size, int depth, int first_of_pointers){  //creation of bucket
  if(indexCell == NULL)
    printf("initIndexCell :: indexCell == NULL \n");
  else{
    indexCell->bucket = (Bucket *)malloc(sizeof(Bucket)*1);
    initBucket(indexCell->bucket, max_size, depth, first_of_pointers);
  }
}

void initIndexCell2(IndexCell *indexCell, IndexCell * const indexCell2){  //copy pointer to same bucket
  if(indexCell2 == NULL)
    printf("initIndexCell :: indexCell2 == NULL \n");
  else{
    indexCell->bucket = indexCell2->bucket;
  }
}

void addToIndexCell(IndexCell *indexCell, Entry *entry){
  if(indexCell == NULL)
    printf("addToIndexCell :: indexCell == NULL \n");
  else{
    addToBucket(indexCell->bucket, entry);
  }
}

void printIndexCell(IndexCell *indexCell){
  if(indexCell == NULL)
    printf("printIndexCell :: indexCell == NULL \n");
  else{
    printf("printing an indexCell...\n");
    printBucket(indexCell->bucket);
  }
}


///////////////////////////////////////////////////////////
void initBucket(Bucket *bucket, int max_size, int depth, int first_of_pointers){
  if(bucket == NULL)
    printf("initBucket :: bucket == NULL\n");
  else{
    bucket->entries = (Entry **)calloc(max_size,sizeof(Entry*));
    
    bucket->curr_size = 0;
    bucket->depth = depth;
    bucket->first_of_pointers = first_of_pointers;  //index no
  }
}

bool fullBucket(Bucket *bucket,int bucketSize){
  bool retValue;
  if(bucket == NULL){
    printf("fullBucket :: bucket == NULL\n");
    retValue = false;
  }
  else{
    if(bucket->curr_size < bucketSize)
      retValue = false;
    else
      retValue = true;
  }
  return retValue;
}

void addToBucket(Bucket *bucket, Entry *entry){
  if(bucket == NULL)
    printf("addToBucket :: bucket == NULL\n");
  else{
    bucket->entries[bucket->curr_size] = entry;
    bucket->curr_size++;
  }
}

void printBucket(Bucket *bucket){
  int i;
  if(bucket == NULL)
    printf("printBucket :: bucket == NULL\n");
  else{
    printf("printing a bucket depth %d..\n",bucket->depth);
    for(i = 0; i < bucket->curr_size; i++){
      printEntryKey(bucket->entries[i]);
      printf("\n");
    }
  }
}

////////////////////////////////////////////////////////////
void initEntry(Entry *entry){
  if(entry == NULL)
    printf("initEntry :: entry == NULL\n");
  else{
    entry->key = -1;
    entry->pointer = NULL;
  }
}

void setEntry(Entry *entry, int key, void *pointer){
  if(entry == NULL)
    printf("setEntry :: entry == NULL\n");
  else{
    entry->key = key;
    entry->pointer = pointer;
  }
}

void printEntryKey(Entry *entry){
  if(entry == NULL)
    printf("printEntry :: entry == NULL\n");
  else
    printf("Entry : %d\n", entry->key);
}
