# HTTP Proxy
This project is a proxy written in C language to provide non-security communications over HTTP requests. It supports regular GET and POST request. It is built over TCP protocols and incorporate a least recently used cache to expediate the repeted requests. 

## Design 
- proxy.h: handles connections with server 
- proxy_cache.h: provides caching features for proxy communications 
- file_node.h: indepedant data structure that build towards the proxy_cache
- parser_helper.h: parse http response returned from server and client http request 

## Usage
Run the program on your machine with ```./a.out [port number]```.

