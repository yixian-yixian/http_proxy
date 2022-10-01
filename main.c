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

#include "proxy.h"
#include "proxy_cache.h"

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

    portno = atoi(argv[1]);
    if (portno < 9195 || portno > 9199) { 
        /* error checking on requested port number */
        fprintf(stderr, "error: port number for this proxy must be in range 9195 to 9199\n");
        exit(1);
    }
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

    /* track system time since clock begins */
    struct timespec trackTime = {0, 0}; 
    /* initialize proxy cache */
    Cache proxy_cache = initializeCache(10);
    clock_t track;
    track = clock();
    /* listen on target port number */
    if (listen(parentfd, 1) < 0) error("ERROR on listen");
    clientlen = sizeof(clientaddr);

    while(1) {
        clock_gettime(CLOCK_MONOTONIC, &trackTime);
        childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (childfd < 0) {
            error("ERROR on accept");
        } 
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL) error("ERROR on gethostbyaddr");
        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL) error("ERROR on inet_ntoa\n");
        printf("server established connection with %s (%s)\n", 
                hostp->h_name, hostaddrp);

        /* clean to prepare for reading */
        bzero(buf, BUFSIZE);
        n = read(childfd, &buf, BUFSIZE);
        if (n < 0) error("ERROR reading from socket");
        printf("server received %d bytes: %s", n, buf);
        
        /* parsing hostname to enable cache functionality */
        char *hostname = calloc(100, sizeof(char));
        char *contentkey = NULL;
        int requestPort;
        parseHostName(buf, hostname, &requestPort);
        printf("hostname [%s], port number [%d] \n", hostname, requestPort);
        createContentKey(&contentkey, buf); 
        void *responsebody = NULL, *output = NULL;
        Node node_add = findNode(&proxy_cache, contentkey);
        float max_age = 0;
    
        if (node_add != NULL) {
            printf("\nREAD FROM CACHE\n");
            if (isStale((float)trackTime.tv_nsec, node_add)){
                printNode(node_add);
                /* send request to server */
                printf("error here \n");
                responseSize = sendtoServer(hostname, requestPort, buf, &responsebody);
                max_age = parseMaxAge(responsebody);
                updateNodeinCache(&proxy_cache, node_add, responsebody, responseSize, (float)trackTime.tv_nsec, max_age);
            }
            node_add = retreiveOnce(&proxy_cache, node_add, &responsebody);
            outputSize = createCacheHeader(node_add, &output, node_add->maxAge - ((float)trackTime.tv_nsec - node_add->entryTime)/pow(10,9));
            n = write(childfd, output, outputSize);
            
        } else{
            printf("\nREAD FROM SERVER\n");
            /* send request to server */
            responseSize = sendtoServer(hostname, requestPort, buf, &responsebody);
            printf("received response size %d\n", responseSize);
            max_age = parseMaxAge(responsebody);
            createNewNode(&proxy_cache, contentkey, responsebody, responseSize, (float)trackTime.tv_nsec, max_age);
            n = write(childfd, responsebody, responseSize);
        }

        // // let's send the actual server response back to the client 
        
        if (n < 0) error("ERROR writing to socket");
        close(childfd);
        printlist(proxy_cache.getHead);
        free(contentkey);
    }
    cleanCache(proxy_cache);

    
}
