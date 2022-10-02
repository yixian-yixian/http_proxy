#include "parser_helper.h"

#define BUFSIZE 1024
#define ContentLength "Content-Length: "
#define HeaderEnd "\r\n\r\n"
#define Protocol "HTTP/1.1"
#define CACHE_AGE "Cache-Control: max-age="
#define HOST "Host: "


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
  char contentLength[BUFSIZE];
  void *intermediate = NULL;
  intermediate = strstr(buf, ContentLength);
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

/* parseHostName 
 * purpose: parse the Host field in the HTTP header
 * prereq: Host is present as a HTTP field 
 * return: None 
 * note: 
 *    hostname will be updated with host field 
 *    port_number will be updated with provide port number,
 *             (default to 80 if not provided)
*/
void parseHostName(void *request_header, char **hostname, int *port_number)
{
    char *host_field = "Host: ";
    char portNumber[10];
    char response[BUFSIZE];
    void *intermediate = strstr(request_header, host_field);
    assert(intermediate != NULL);
    intermediate += strlen(host_field);
    sscanf(intermediate, "%[^\r\n]", response);
    int found = parsePortNumber(response, port_number);
    if (found) sscanf(response, "%[^:]", *hostname);
    else memcpy(*hostname, response, strlen(response));
    

}


/* parsePortNumber 
 * purpose: parse the port number from HTTP header in 
 *          client's request
 * return: if specified, the port number is returned
 *         else, default SERVER_PORT is returned 
 * param:
 *      buf: http request sent from client
*/
int parsePortNumber(void *hostname, int *portnumber)
{
  char portNumber[10];/* TCP maximum port number is 65,535 */
  
  int index = 0;
  void *protocol = strstr(hostname, ":");
  if (protocol == NULL) {
    *portnumber = SERVER_PORT;
    return 0;
  } 
  
  while (index < strlen(hostname)){
    if (*(char *)(hostname + index) == ':'){
      memcpy(portNumber, hostname + index + 1, strlen(hostname) - index - 1);
      *portnumber = atoi(portNumber);
      break;
    } 
    index += 1;
  }

  
  return 1;
  
}

/* createContentKey 
 * purpose: parse out the request address as the contentkey
 *          for proxy cache storage
 * return:  None
 * param: 
 *      char **contentKey: pointer to contentKey 
 *      void *buf: buffer content that parsing is completed on
*/
void createContentKey(char **contentKey, void *buf)
{
  int index = 0;
  *contentKey = calloc(100, sizeof(char));
  sscanf(buf+11, "%[^ HTTP]", *contentKey);
  
  
}

/* parseMaxAge 
 * purpose: parse the max-age field in the HTTP response header
 * return:  max-age provided by the HTTP response header
 * param: 
 *      void *buf: buffer content that parsing is completed on
 *
*/
int parseMaxAge(void *buf)
{
  assert(buf != NULL);
  char ageField[BUFSIZE];
  void *endofHeader = strstr(buf, CACHE_AGE);
  if (endofHeader == NULL) {
    return 0;
  } else {
    endofHeader += strlen(CACHE_AGE);
    sscanf(endofHeader, "%[^\r\n]", ageField);
    printf("what is read %s\n",ageField);
    return atoi(ageField);
  }
}


int divRoundClosest(const long n, const long d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}
