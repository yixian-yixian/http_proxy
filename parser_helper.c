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
 * purpose: parse the port number from HTTP header in 
 *          client's request
 * return: if specified, the port number is returned
 *         else, default SERVER_PORT is returned 
 * param:
 *      buf: http request sent from client
*/
int parsePortNumber(void *buf)
{
  assert(buf != NULL);
  char portNumber[10];/* TCP maximum port number is 65,535 */
  void *protocol = strstr(buf, Protocol);/* guaranteed for legal HTTP response */
  void *startIndex = protocol;
  while(*(char *)startIndex != '/') {/* iterate backwards from HTTP/1.1 */
    if (*(char *)startIndex == ':' ){/* legal portnumber follows semicolon */
      /* drop the space character */
      sscanf(startIndex + 1, "%[^ HTTP]", portNumber);
      return atoi(portNumber);
    }
    startIndex -= 1;
  }
  return SERVER_PORT;
  
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
    return atoi(ageField);
  }
}

