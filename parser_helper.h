#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define SERVER_PORT 80



size_t parseContentLength(void *buf);
size_t parseHttpHeader(void *buf);
int    parsePortNumber(void *buf);
void   createContentKey(char **contentKey, void *buf);
float  parseMaxAge(void *buf);