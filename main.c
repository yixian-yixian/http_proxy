#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#include "proxy.h"
#include "proxy_cache.h"
#include "parser_helper.h"

int main(int argc, char **argv)
{
    /* check command line arguments */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int parentfd, childfd; /* variable for server and client sockets */
    int portno, clientlen; /* portnumber and byte size of client's address */
    int optval = 1; /* flag value for setsockopt*/
    int n; /* variable to hold request message byte size */
    struct sockaddr_in serveraddr, clientaddr; /* server's and client's addr */
    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
    char buf[BUFSIZE]; /* message buffer */
    size_t responseSize = 0, outputSize = 0;
    int max_age = 0; /* variable to hold maximum age field if server provided */

    portno = atoi(argv[1]);
    /* create a parent socket */
    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) error("ERROR opening socket");

    /* setsockopt: allow immediate rerun on the requested server */
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
	    (const void *)&optval , sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; /* set an internet address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);/* allow system set proxy IP address */
    serveraddr.sin_port = htons((unsigned short)portno); /* set listening port number */

    /* binding associated socket to socket server */
    if (bind(parentfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) error("ERROR on binding");

    /* initialize proxy cache */
    Cache proxy_cache = initializeCache(10);

    /* listen on target port number */
    if (listen(parentfd, 1) < 0) error("ERROR on listen");
    clientlen = sizeof(clientaddr);


    while(1) {

        /* proxy has received request from client */ 
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0) {
            fprintf(stderr, "ERROR on accept\n");
            continue;
        } 

        /* track system time since clock begins */
        struct timespec trackTime = {0, 0}; 
        clock_gettime(CLOCK_MONOTONIC, &trackTime);
        long int currentTime = trackTime.tv_sec*(1.0e9) + trackTime.tv_nsec;
    

        /* Setting up information for client server */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL) {
            fprintf(stderr, "ERROR on gethostbyaddr");
            continue;
        }
        
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL){
            fprintf(stderr, "ERROR on inet_ntoa\n");
            continue;
        } 


        /* clean to prepare for reading */
        bzero(buf, BUFSIZE);
        n = read(childfd, &buf, BUFSIZE);
        if (n < 0) {
            fprintf(stderr,"ERROR reading from socket");
            continue;
        }
        
        /* parsing hostname to enable cache functionality */
        char *hostname = calloc(100, sizeof(char));
        char *contentkey = NULL;
        int requestPort;
        parseHostName(buf, &hostname, &requestPort);
        printf("hostname [%s], request port [%d]\n", hostname,requestPort);
        createContentKey(&contentkey, buf); 
        printf("contentkey generated %s\n", contentkey);
        void *responsebody = NULL, *output = NULL;
        Node node_add = findNode(&proxy_cache, contentkey);
        
        if (node_add != NULL && isStale(currentTime, node_add) == false) { /* fresh node in cache */
            printNode(node_add);
            node_add = retreiveOnce(&proxy_cache, node_add, &responsebody);
            /* Age = maxAge - (time-elapsed at this request - entryTime) */
            int macro = divRoundClosest((currentTime - node_add->entryTime), 1.0e9);
            outputSize = createCacheHeader(node_add, &output, macro);
            n = write(childfd, output, outputSize);
        } else {
            /* stale and nonpresent nodes both require additional update in age and time field */
            if (node_add != NULL) removeNode(node_add);
            /* first send request to server */
            responseSize = sendtoServer(hostname, requestPort, buf, &responsebody);
            if (responseSize < 0) {
                error("ERROR on communication with request server\n");
            }
            max_age = parseMaxAge(responsebody);
            n = write(childfd, responsebody, responseSize);
            node_add = createNewNode(&proxy_cache, contentkey, responsebody, responseSize, currentTime, max_age);
        }

        if (n < 0) error("ERROR writing to socket");
        close(childfd);
        free(contentkey);
        free(hostname);
        if (output != NULL) free(output);
    }
    cleanCache(proxy_cache);

    
}
// int main(void){
//     char *target = "Cache-Control: max-age=  980 \r\n\0";
//     char *lowercase;
//     int age = parseMaxAge(target);
//     // convertAlltolowercase(target, &lowercase);
//     // printf("%s\n", lowercase);
// }