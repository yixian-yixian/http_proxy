#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define SERVER_PORT 80



size_t  parseContentLength(void *buf);
size_t  parseHttpHeader(void *buf);
int     parsePortNumber(void *hostname, int *portnumber);
void    createContentKey(char **contentKey, void *buf);
int     parseMaxAge(void *buf);
void    parseHostName(void *request_header, char **hostname, int *port_number);
int     divRoundClosest(const long n, const long d);