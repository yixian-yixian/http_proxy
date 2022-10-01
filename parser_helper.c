#include "parser_helper.h"

#define BUFSIZE 1024
#define ContentLength "Content-Length: "
#define HeaderEnd "\r\n\r\n"
#define Protocol "HTTP/1.1"
#define CACHE_AGE "Cache-Control: maxAge="


char *
substr(const char *src, size_t start, size_t len)
{
  char *dest = malloc(len+1);
  if (dest) {
    memcpy(dest, src+start, len);
    dest[len] = '\0';
  }
  return dest;
}

/* parseContentLength
 * purpose: parse the Content-Length field of the HTTP header
 * prereq: Content-Length is guaranteed to be present in buf
 * return: Content-Length in size_t unit
 * param: 
 *    buf: starting index of the entire buffer
*/
size_t parseContentLength(void *buf)
{
  printf("isseu is in parse_helper");
  char contentLength[BUFSIZE];
  char *intermediate = strstr(buf, ContentLength);
  intermediate += strlen(ContentLength);
  sscanf(intermediate, "%[^\r\n]", contentLength);
  return atoi(contentLength);
}

/* parseHttpHeader
 * purpose: find the pointer to the end of header field 
 *          including the \r\n in the final entry of header
 * return: if end of header is found, return total number 
 *            of bytes in the header field;
 *         if not found, -1 is returned 
 * param: 
 *    buf: starting index of the entire buffer 
 *    endPos: ending pointer of the http header
 */
size_t parseHttpHeader(void *buf)
{
  assert(buf != NULL);
  void *endofHeader = strstr(buf, HeaderEnd);
  return (endofHeader != NULL) ? (endofHeader - buf + 2) : -1;
}



/* parsePortNumber 
 * purpose: parse the port number from HTTP header
 * return: if specified, the port number is returned
 *          else, default SERVER_PORT is returned 
 * param:
 *      buf: http request sent from client
*/
int parsePortNumber(void *buf)
{
  assert(buf != NULL);
  buf += '\0';
  char portNumber[10];/* TCP maximum port number is 65,535 */
  void *protocol = strstr(buf, Protocol);
  buf -= '\0';
  if (protocol == NULL) return SERVER_PORT;
  void *startIndex = protocol;
  while(*(char *)startIndex != '/') {
    if (*(char *)startIndex == ':' ){
      sscanf(startIndex+1, "%[^ HTTP]", portNumber);
      return atoi(portNumber);
    }
    startIndex -= 1;
  }
  return SERVER_PORT;
  
}

// TODO fix hostname to get the entire link and then port
void createContentKey(char **contentKey, void *buf)
{
  // char portNumber[10];
  // *contentKey = calloc(100, sizeof(char));
  // memcpy(*contentKey, hostname, strlen(hostname));
  // sprintf(portNumber, ":%d", portnumber);
  // strncat(*contentKey, portNumber, strlen(portNumber));
  // printf("current key is %s", *contentKey);
  *contentKey = calloc(100, sizeof(char));
  sscanf(buf+12, "%[^ HTTP]", *contentKey);
  printf("contentkey generated is %s\n", *contentKey);
  
}

float parseMaxAge(void *buf)
{
  assert(buf != NULL);
  char ageField[BUFSIZE];
  void *endofHeader = strstr(buf, CACHE_AGE);
  if (endofHeader == NULL) {
    return 0;
  } else {
    endofHeader += strlen(CACHE_AGE);
    sscanf(endofHeader, "%[^\r\n]", ageField);
    printf("read %d",atoi(ageField));
    return (float)atoi(ageField);
  }
  
}

