#ifndef CACHE_INCLUDED
#define CACHE_INCLUDED
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "file_node.h"

typedef struct cache Cache;
typedef Cache* Cache_T;


struct cache {
    size_t currSize;
    size_t cap;
    Node getHead, getTail;
};

Node createNewNode(Cache_T ORG, char *name, void *inputContent, size_t contentSize, long entryTime, int maxAge);

Cache initializeCache(size_t capacity);
void cleanCache(Cache ORG);
// void updateNodeinCache(Cache_T ORG, Node target, void *responseContent, size_t contentSize, long currTime, int max_age);
Node retreiveOnce(Cache_T ORG, Node target, void **responsebody);
Node findNode(Cache_T ORG, char *keyName);
void evictCache(Cache_T ORG, float currTime);
bool isStale(long currTime, Node target);
bool shouldEvict(Cache_T ORG);
void printlist(Node head);



#endif