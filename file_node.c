#include "file_node.h"

/* initNode 
 * purpose: construct a node class on heap memory and return pointer
 *          to the existing object 
 * pre-req: none
 * return: pointer to an initialized linkedNode object 
 * param:
 *          name: filename, serving as contentKey
 *          inputContent: the bytes of contents associated with contentKey
 *          maxAge: maximum age to live for the present file
 *          entryTime: initial storage time of the file          
 */

Node initNode(char *name, void *inputContent, int maxAge, float entryTime, size_t contentSize)
{
    Node prod = malloc(sizeof(struct linkedNode));
    char *storeFilename = calloc(strlen(name)+1, sizeof(char)); 
    prod->fileName = strcpy(storeFilename, name);
    prod->fileContent = inputContent;
    prod->entryTime = entryTime;
    prod->maxAge = maxAge;
    prod->contentSize = contentSize;
    prod->prev = NULL;
    prod->next = NULL;
    return prod;
}

/* freeNode 
 * purpose: remove all heap memory of the target linkedNode object
 * pre-req: target is an initialized object on heap memory
 */
void freeNode(Node target)
{
    assert(target != NULL);
    free(target->fileName);
    if (target->fileContent != NULL) free(target->fileContent);
    free(target);
}

/* freeLinkedlist 
 * purpose: remove all nodes linked by the provided head node 
 * preq-req: head node is initialized and not NULL 
 */
void freeLinkedlist(Node head)
{
    assert(head != NULL);
    Node curr = head;
    Node temp;
    while (curr != NULL) {
        temp = curr;
        curr = curr->next;
        freeNode(temp);
    }
}

/* putNewNode 
 * purpose: insert the node_ptr into the linkedlist led by head node 
 * preq-req: both nodes are present
*/
void putNewNode(Node head, Node node_ptr) 
{
    Node temp = head->next;
    head->next = node_ptr;
    temp->prev = node_ptr;
    node_ptr->prev = head;
    node_ptr->next = temp;
}

/* removeNode 
 * purpose: remove a target node from its existing linkedlist 
 * preq-req: both nodes are present
*/
void removeNode(Node node_ptr)
{
    Node prior = node_ptr->prev;
    Node next = node_ptr->next;
    prior->next = next;
    next->prev = prior;
    freeNode(node_ptr);
}


/* movetoHead 
 * purpose: remove a node from its original list and place the node into 
 *          the head of another list, update the Node with this entryTime
 * prereq: node target and head are both valid address 
 * return: None
 * param: 
*/
Node movetoHead(Node head, Node target)
{
    void *newContent = malloc(target->contentSize);
    memcpy(newContent, target->fileContent, target->contentSize);
    Node curr = initNode(target->fileName, newContent, target->maxAge, target->entryTime, target->contentSize);
    removeNode(target);
    putNewNode(head, curr);
    printf("insde move tohead \n\n");
    printNode(curr);
    return curr;
}

/* popTail
 * purpose: remove a node from the back of a linkedlist
 * use case: remove the least recently used PUT node or GET node in situation without
 *          stale nodes
*/
void popTail(Node tail)
{
    Node target = tail->prev;
    removeNode(target);
}

/* updateNode
 * purpose: update a target node's certain field with new value
 * use case: a file node is PUT again and with new content and information
*/
void updateNode(Node target, void *content, int maxAge, size_t contentSize, float entryTime)
{
    free(target->fileContent);
    target->fileContent = content;
    target->maxAge = maxAge;
    target->contentSize = contentSize;
    target->entryTime = entryTime;
}

/* BELOW HELPER FUNCTION TO BE CLEANED UP AND REMVOED LATER  */

void printNode(Node target)
{
    printf("\n");
    printf("Node %s with size %d \n", target->fileName, target->contentSize);
    printf("current node age %d \n", target->maxAge);
    if (target->prev != NULL) {
        printf("prev value %s\n", target->prev->fileName);
    } else {
        printf("prev value empty\n");
    }

    if (target->next != NULL) {
        printf("next value %s\n", target->next->fileName);
    } else {
        printf("next value empty \n");
    }

    printf("\n ");
}
