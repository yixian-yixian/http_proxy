#ifndef FILE_NODE_INCLUDED
#define FILE_NODE_INCLUDED

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h> 
#include <string.h>
#include <unistd.h> 

typedef struct linkedNode* Node;

struct linkedNode{
    char *fileName;
    void *fileContent;
    float entryTime;
    int maxAge;
    size_t contentSize;
    Node prev;
    Node next;
};


Node initNode(char *name, void *inputContent, int maxAge, float entryTime, size_t contentSize);
void freeNode(Node target);
void updateNode(Node target, void *content, int maxAge, size_t contentSize, float entryTime);
void putNewNode(Node head, Node node_ptr);
void removeNode(Node node_ptr);
void freeLinkedlist(Node head);
Node movetoHead(Node head, Node target);
void popTail(Node tail);
void printNode(Node target);



#endif