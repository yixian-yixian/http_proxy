#include "proxy.h"
#define DEFAULT_SIZE 10485760
#define CACHE_AGE "Cache-Control"
/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}


/* 
 * signalExit wrapper for proxy to return to listen mode 
 */
int signalExit(char *msg){
    perror(msg);
    return -10;
}


/* readResponseFromServer 
 * purpose: read all bytes returned from server side 
 * return: size of header field in buffer 
 * param: 
 *    int fd: file descriptor to the proxy's client
 *            socket to server 
 *    void **buffer_addr: address of buffer to store response
 *                        locally
*/
size_t readResponseFromServer(int fd, void **buffer_addr)
{
  printf("server not responding?");
  size_t totalBytesRead = 0, chunk = 0, headerSize = 0;
  void *temporaryBuf = malloc(sizeof(char) * DEFAULT_SIZE);
  chunk = read(fd, temporaryBuf, BUFSIZE);
  while ((headerSize = parseHttpHeader(temporaryBuf)) == -1) {
    printf("inside first loop \n");
    totalBytesRead += chunk;
    chunk = read(fd, temporaryBuf + totalBytesRead, DEFAULT_SIZE - totalBytesRead);
  }
  size_t contentLength = parseContentLength(temporaryBuf);
  printf("contentLength successfully read\n");
  while(totalBytesRead <= contentLength && chunk > 0){
    printf("currently reading %d", chunk);
    totalBytesRead += chunk;
    chunk = read(fd, temporaryBuf + totalBytesRead, DEFAULT_SIZE - totalBytesRead);
    // move the chunk break initailly here to outside while loop
    if (chunk < 0) return -1;
  }
  
  *buffer_addr = temporaryBuf;
  return totalBytesRead;
  
}

/* createCacheHeader 
 * purpose: prepare HTTP response information for client
 *          to indicate that this content was retrieved
 *          from cache 
 * return:  overall size of the HTTP repsonse with body 
 *          content 
 * param:  
 *          Node outputNode: previously cache HTTP response
 *                           from server associated with 
 *                           a client's prior request
 *                           request 
 *          void **output:   pointer to the HTTP response 
 *          float age:       maximum age to live for a 
 *                           HTTP request in cache
 */

size_t createCacheHeader(Node outputNode, void **output, int age)
{
  assert(outputNode != NULL);
  char cacheField[BUFSIZE];
  sprintf((char *)cacheField, "Age: %d\r\n\0", age);
  size_t cacheFieldSize = strlen(cacheField);
  size_t headerSize = parseHttpHeader(outputNode->fileContent); 
  size_t bodySize = parseContentLength(outputNode->fileContent) + 2; 
  size_t totalSize = headerSize + cacheFieldSize + bodySize;
  *output = malloc(sizeof(char) * totalSize);
  memcpy(*output, outputNode->fileContent, headerSize);
  memcpy(*output + headerSize, cacheField, cacheFieldSize);
  memcpy(*output + headerSize + cacheFieldSize, outputNode->fileContent + headerSize, bodySize);
  return totalSize;
}



size_t sendtoServer(char *hostname, int portno, void *buf, void **response)
{
    int sockfd, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    printf("host name is %s\n", hostname);
    server = gethostbyname(hostname);

    if (server == NULL) {
      error("ERROR, no such host as %s\n");
  
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");


    /* send request to server */
    n = write(sockfd, buf, strlen(buf));
    if (n < 0) error("ERROR writing to socket");
    bzero(buf, BUFSIZE);

    /* read response from server */
    n = readResponseFromServer(sockfd, response);

    
    if (n < 0) error("ERROR reading from socket");
    
    close(sockfd);

    return n;

}


