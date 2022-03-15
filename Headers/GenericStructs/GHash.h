#ifndef HASH_H
#define HASH_H
#include <stdbool.h>
#include <stdint.h>  /* Replace with <stdint.h> if appropriate */

typedef struct HashTable HashTable;
typedef struct IndexCell IndexCell;
typedef struct Bucket Bucket;
typedef struct Entry Entry;

int hashFunction(Entry *entry, HashTable *hashTable);
uint32_t hashFunction2(uint32_t key,HashTable *hashTable);
uint32_t hashFunction3(char* str,HashTable *hashTable);
uint32_t superFastHash (long int key,HashTable *hashTable);


struct HashTable {
  IndexCell *index;
  int index_size;
  int bucketSize;
  int depth;
};

void initHashTable(HashTable *hashTable, int init_size, int bucket_max_size);
void initHashTable2(HashTable *hashTable, int init_size, int bucket_max_size);
void doubleIndex(HashTable *hashTable);
int addToHashTable(HashTable *hashTable, Entry *entry);
int addToHashTableStringKey(HashTable *hashTable, Entry *entry);
void printHashTable(HashTable *hashTable);
HashTable* expandHash(HashTable *hashTable); // Apo edw Eixe to Entry* 
HashTable* spiltBucket(HashTable *hashTable,Bucket * old_bucket); // Apo edw Eixe to Entry*
int destroyHash(HashTable* hashTable);
int destroyHashEntriesValues(HashTable *hashTable);
int destroyHashEntries(HashTable *hashTable);
void* getEntry(HashTable* hashTable, int key );
int getEntryStringKey (HashTable* hashTable, char* key );
struct IndexCell {  //NEEDED
  Bucket *bucket;
};

void initIndexCell(IndexCell *indexCell, int max_size, int depth, int first_of_pointers);
void initIndexCell2(IndexCell *indexCell, IndexCell * const indexCell2);
void addToIndexCell(IndexCell *indexCell, Entry *entry);
void printIndexCell(IndexCell *indexCell);

struct Bucket {
  Entry **entries;
  int curr_size;
  int depth;
  int first_of_pointers;
};

void initBucket(Bucket *bucket, int max_size, int depth, int first_of_pointers);
bool fullBucket(Bucket *bucket,int bucketSize);
void addToBucket(Bucket *bucket, Entry *entry);
void printBucket(Bucket *bucket);

struct Entry {
  int key;
  void *pointer;
};

void initEntry(Entry *entry);
Entry* createEntry(int key, void* value);
void setEntry(Entry *entry, int key, void *pointer);
void printEntryKey(Entry *entry);

#endif


