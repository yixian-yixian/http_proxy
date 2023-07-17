#include "proxy_cache.h"

#define MAX_AGE 3600

Node findOldestStaleinGET(Cache_T ORG, float currTime);
Node findNodeinLL(Node head, char *fileName);

/* createNewNode(char *name, void *inputContent, int maxAge, 
 *              float entryTime, size_t contentSize)
 * purpose: create a new node in cache
 */
Node 
createNewNode(Cache_T ORG, char *name, void *inputContent, size_t contentSize, long entryTime, int maxAge)
{
    if (shouldEvict(ORG)){ /* check if full cache */
        evictCache(ORG, entryTime);
    } /* new insertion for absent filenode */
    if (maxAge == 0) maxAge = MAX_AGE;
    Node node_add = initNode(name, inputContent, maxAge, entryTime, contentSize);
    putNewNode(ORG->getHead, node_add);
    ORG->currSize++;
    return node_add;
}


/* isStale()
 * purpose: check if the provided target node has gone stale according
 *          to the parameter currentTime and the record maxAge
 * prereq: target must be an initialized node
 * return: True if the node has gone stale,  False otherwise 
 * parameter: 
 *         currTime: current time in float unit of the operation
 *          target: pointer to the target filenode
*/
bool 
isStale(long currTime, Node target)
{
    assert(target != NULL);
    long timeElapsed = (currTime - target->entryTime) / 1.0e9;
    if (timeElapsed > target->maxAge) return true;
    return target->maxAge == 0 ? true : false;
}


/* initializeCache 
 * purpose: initialize a cache of target capacity
 * prereq: capacity is a nonnegative value 
 * return: initialized cache object
 * parameter: 
 *      capacity: requested size of the Cache
*/ 
Cache 
initializeCache(size_t capacity)
{
    Cache ORG;
    ORG.currSize = 0;
    ORG.cap = capacity;
    ORG.getHead = initNode("GET HEAD NODE", NULL, 0,0,0);
    ORG.getTail = initNode("GET TAIL NODE", NULL, 0,0,0);
    ORG.getHead->next = ORG.getTail;
    ORG.getTail->prev = ORG.getHead;
    return ORG;
}

/* cleanCache ()
 * purpose: remove the putList and getList from heap memories 
 * prereq: ORG is an initialized cache with putList and getList 
 *          on heap memories 
 * return: None 
 * parameter: 
 *      ORG: an initialized cache object 
*/
void 
cleanCache(Cache ORG){
    freeLinkedlist(ORG.getHead);
}



/* findNode
 * purpose: iterate through both list to identify any Node that has 
 *          the same fileName
 * prereq: Cache_T must be an address of an initialized Cache struct 
 * return: pointer to the target Node struct; NULL if empty or not found
 * parameter: 
 *         ORG: pointer to the cache object with two list
 *         keyName: target filename that we are looking for
 */
Node 
findNode(Cache_T ORG, char *keyName)
{
    Node target = NULL;
    /* uninitailized empty cache */
    if (ORG->currSize == 0) return NULL;
    /* iterate to identify if node is present in list */
    target = findNodeinLL(ORG->getHead, keyName);
    return target;
}

/* retrieveOnce
 * purpose: retrieve an existing node in the cache once 
 *          and move it to head following LRU cache logic
 * prereq: ORG is initialized and target is present in cache
 * parameter: 
 *          Cache_T ORG: pointer to the cache object 
 *          Node target: pointer to the target filenode
 *          void **responsebody: pointer to the filecontent
 * notes: **responsebody now points to the filecontent on heap
 *        memory
 *
*/
Node 
retreiveOnce(Cache_T ORG, Node target, void **responsebody)
{
    assert(target != NULL);
    Node newPos = movetoHead(ORG->getHead, target);
    *responsebody = newPos->fileContent;
    return newPos;
}



/* findOldestStale
 * purpose: iterate through both lists to identify the oldest stale item
 *          and update the cache for the total number of stale items
 * prereq: Cache_T must be an address of an initialized Cache struct 
 * return: pointer to the oldest state Node; NULL if none were stale
 * parameter: 
 *         ORG: pointer to the cache object with two list
 *         currTime: current CPU time of the operation 
 * notes: update the total number of stale items record in Cache
 */
Node 
findOldestStale(Cache_T ORG, float currTime)
{
    Node oldest = findOldestStaleinGET(ORG, currTime);
    return oldest;
}


/* evictCache
 * purpose: following the evicting cache policy to consider which node to evict 
 *          first priority -> remove oldest stale nodes
 *          second priority -> remove oldest nonretrieved node (from putList)
 *          third priority -> remove oldest retrieved node (lru node from getList)
 * prereq: ORG has to be an initialized Cache 
 * return: None 
 * parameter: 
 *      ORG: pointer to an initialized cache object 
 *      currTime: current time in float unit of the operation
*/
void 
evictCache(Cache_T ORG, float currTime)
{
    Node oldestStale = findOldestStale(ORG, currTime);
    /* remove oldest stale node if the cache is full */
    if (oldestStale != NULL) {
        removeNode(oldestStale);
    } else { /* remove the oldest non-retrieved one first */
        popTail(ORG->getTail);
    }
    ORG->currSize -= 1;
}


/* shouldEvict()
 * purpose: check if the provided cache has a full capacity cache 
 * prereq: Cache_T must be an address of an initialized Cache struct 
 * return: True if Cache is at full capacity, False if not
 * parameter: 
 *         ORG: pointer to the cache object with two list
*/
bool 
shouldEvict(Cache_T ORG)
{
    return (ORG->currSize >= ORG->cap) ? true : false;
}

/*  * * * * * * * * Local helper functions  * * * * * * * * * * * * */
/* findNodeinLL
 * purpose: iterate through a linkedlist to identify any Node that has
 *          the same fileName
 * prereq: Cache_T must be an address of an initialized Cache struct 
 * return: pointer to the target Node struct; NULL if not found
 * parameter: 
 *         ORG: pointer to the cache object with two list
 *         keyName: target filename that we are looking for
 */
Node 
findNodeinLL(Node head, char *fileName)
{
    while(head != NULL){
        if (strcmp(head->fileName, fileName) == 0) break;
        head = head->next;
    }
    return (head != NULL) ? head : NULL;
}

/* findOldestStaleinGET
 * purpose: iterate through putlist to identify the oldest stale item
 *          and update the cache for the total number of stale items
 * prereq: Cache_T must be an address of an initialized Cache struct 
 * return: pointer to the oldest state Node; NULL if none were stale
 * parameter: 
 *         ORG: pointer to the cache object with two list
 *         currTime: current CPU time of the operation 
 * notes: update the total number of stale items record in Cache
 */
Node 
findOldestStaleinGET(Cache_T ORG, float currTime)
{
    float maxTimeDiff = 0.0;
    Node curr = ORG->getHead->next;
    Node oldest = NULL;
    while(curr != ORG->getTail){
        if (isStale(currTime, curr)){
            float timeElapsed = (currTime - curr->entryTime) / pow(10,9);
            if (timeElapsed > maxTimeDiff) {
                oldest = curr; 
                maxTimeDiff = timeElapsed;
            }
        }
        curr = curr->next;
    }
    return (oldest != ORG->getHead) ? oldest : NULL;
}

// HELPER FUNCTION TO BE REMOVED 
void printlist(Node head)
{
    Node curr = head;
    while (curr != NULL) {
        printNode(curr);
        curr = curr->next;
    }
}

