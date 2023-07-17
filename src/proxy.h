#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <math.h>

#include "parser_helper.h"
#include "proxy_cache.h"
#include "file_node.h"

#define SERVER_PORT 80
#define BUFSIZE 1024

void error(char *msg);
size_t sendtoServer(char *hostname, int portno, void *buf, void **response);
size_t readResponseFromServer(int fd, void **buffer_addr);
size_t createCacheHeader(Node outputNode, void **output, int age);