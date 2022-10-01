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

void backtoMain(char *msg){
    perror(msg);
}

/* parseHostName 
 * purpose: parse the Host field in the HTTP header
 * prereq: Host is present as a HTTP field 
 * return: None 
 * note: 
 *    hostname will be updated with host field 
 *    port_number will be updated with provide port number,
 *             (default to 80 if not provided)
*/
void parseHostName(void *request_header, char *hostname, int *port_number)
{
    char *host_field = "Host: ";
    void *intermediate = strstr((char *)request_header, host_field);
    assert(intermediate != NULL);
    intermediate += strlen(host_field);
    sscanf(intermediate, "%[^\r\n]", hostname);
    *port_number = parsePortNumber(request_header);

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
  size_t totalBytesRead = 0, chunk = 0, headerSize = 0;
  void *temporaryBuf = malloc(sizeof(char) * DEFAULT_SIZE);
  chunk = read(fd, temporaryBuf, BUFSIZE);
  while ((headerSize = parseHttpHeader(temporaryBuf)) == -1) {
    totalBytesRead += chunk;
    chunk = read(fd, temporaryBuf + totalBytesRead, DEFAULT_SIZE - totalBytesRead);
  }
  size_t contentLength = parseContentLength(temporaryBuf);
  while(totalBytesRead <= contentLength && chunk > 0){
    totalBytesRead += chunk;
    chunk = read(fd, temporaryBuf + totalBytesRead, DEFAULT_SIZE - totalBytesRead);
    if (chunk < 0) error("ERROR: unable to read from server\n");
  }
  *buffer_addr = temporaryBuf;
  return totalBytesRead;
  
}


/* NEED TO FIX THE MISSING NEWLINE */

size_t createCacheHeader(Node outputNode, void **output, float age)
{
  // FILE *cp;
  // cp = fopen("original_content", "wb");
  // fwrite(outputNode->fileContent, 1, outputNode->contentSize, cp);
  // fclose(cp);
  assert(outputNode != NULL);
  char cacheField[BUFSIZE];
  sprintf((char *)cacheField, "Cache-Control: max-age=%d\r\n\0", (int)age);
  size_t cacheFieldSize = strlen(cacheField);
  // printf("current cache size %d, current cache line %s\n", cacheFieldSize, cacheField);
  size_t headerSize = parseHttpHeader(outputNode->fileContent); 
  // size_t bodySize = parseContentLength(outputNode->fileContent) + 2;
  size_t bodySize = parseContentLength(outputNode->fileContent) + 2; // include the new line between Cache control and hTTP body
  size_t totalSize = headerSize + cacheFieldSize + bodySize;
  *output = malloc(sizeof(char) * totalSize);
  memcpy(*output, outputNode->fileContent, headerSize);
  memcpy(*output + headerSize, cacheField, cacheFieldSize);
  // memcpy(*output + headerSize + cacheFieldSize, outputNode->fileContent + headerSize - 2, bodySize + 2);
  memcpy(*output + headerSize + cacheFieldSize, outputNode->fileContent + headerSize, bodySize);
  
  printf("header size %d, body size %d, content original total size %d, cache size %d", 
        headerSize, bodySize, outputNode->contentSize, cacheFieldSize);

  return totalSize;
}





size_t sendtoServer(char *hostname, int portno, void *buf, void **response)
{
    int sockfd, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
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
    if (n < 0) 
      error("ERROR writing to socket");
    bzero(buf, BUFSIZE);

    /* read response from server */
    n = readResponseFromServer(sockfd, response);
    
    if (n < 0) 
      error("ERROR reading from socket");
    
    close(sockfd);
    printf("returned by sendtoServer %d \n", n);
    return n;


}

