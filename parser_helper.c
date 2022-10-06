#include "parser_helper.h"

#define BUFSIZE 1024
#define CL "Content-Length: "
#define HeaderEnd "\r\n\r\n"
#define Protocol "HTTP/1.1"
#define CACHE_AGE "cache-control: max-age"
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

// TODO - client of the function to free the extra copy
void convertAlltolowercase(void *buf, char **product)
{
  size_t totalLen = strlen(buf), index = 0;
  char *lowercaseString = calloc(totalLen + 1, sizeof(char));
  while (index < totalLen) {
    lowercaseString[index] = tolower(*(char *)(buf + index));
    index += 1;
  }
  *product = lowercaseString;
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
  void *endofHeader = strstr(buf, HeaderEnd);
  void *intermediate = strstr(buf, CL);
  intermediate += strlen(CL);
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
    char *response = calloc(BUFSIZE, sizeof(char));
    void *intermediate = strstr(request_header, host_field);
    assert(intermediate != NULL);/* guaranteed to be present */
    intermediate += strlen(host_field);/* read the entirty of Host line */
    sscanf(intermediate, "%[^\r\n]", response);
    int found = parsePortNumber(response, port_number);
    if (found) sscanf(response, "%[^:]", *hostname);
    else memcpy(*hostname, response, strlen(response));
    char *final_lower_case;
    convertAlltolowercase(*hostname, &final_lower_case);
    free(*hostname);
    *hostname = final_lower_case;
}


/* parsePortNumber 
 * purpose: parse the port number from HTTP header in 
 *          client's request
 * return: if specified, the port number is returned
 *         else, default SERVER_PORT is returned 
 * param:
 *      buf: http request sent from client
*/
int parsePortNumber(char *hostname, int *portnumber)
{
  char port_number_field[10];/* TCP maximum port number is 65,535 */
  
  int index = 0;
  void *protocol = strstr(hostname, ":");
  if (protocol == NULL) {
    *portnumber = SERVER_PORT;
    return 0;
  } 
  memcpy(port_number_field, protocol + 1, strlen(hostname) - (protocol - (void *)hostname));
  printf("port number field %s\n", port_number_field);
  *portnumber = atoi(port_number_field);

  
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
  char *content_key_field = calloc(100, sizeof(char));
  printf("current buf %s\n", buf);
  void *httpField = strstr(buf + 3, "HTTP");
  printf("current buf [%s]\n", buf);
  // int index = 0;
  // char *content_key_field = calloc(100, sizeof(char));
  // void *httpField = strstr(buf + 11, " ");
  printf("http field [%s]\n", httpField);
  memcpy(content_key_field, buf + 4, httpField - (buf + 5));
  
  printf("content_key_field [%s]\n", content_key_field);
  content_key_field = realloc(content_key_field, strlen(content_key_field)+1);
  while (index < strlen(content_key_field)){
    char lowercase = tolower(content_key_field[index]);
    content_key_field[index] = lowercase;
    index += 1;
  }
  *contentKey = content_key_field;
  
  
  
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
  char ageField[BUFSIZE], intermediate[BUFSIZE];
  char *lowercaseHeader = NULL;
  sscanf(buf, "%[^\r\n\r\n]", intermediate);
  convertAlltolowercase(intermediate, &lowercaseHeader);
  void *endofHeader = strstr(lowercaseHeader, CACHE_AGE);
  int age, index = 0;
  if (endofHeader == NULL) {
    age = 0;
  } else {
    endofHeader += strlen(CACHE_AGE);
    while(isdigit(*(char *)endofHeader) == 0) endofHeader += 1;
    sscanf(endofHeader, "%[^\r\n]", ageField);
    printf("what is read %s\n",ageField);
    age = atoi(ageField);
  }
  free(lowercaseHeader);
  return age;
}


int divRoundClosest(const long n, const long d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}
