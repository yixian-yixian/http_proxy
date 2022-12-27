# HTTP Proxy (in C)
This project is a proxy based in C language that provides non-security communications over HTTP requests. It supports regular GET and POST request (another project is https_proxy supports security communications for HTTPS requests).

- proxy.h: handles connections with server 
- proxy_cache.h: facilitates caching communication for proxy establishment 
- file_node.h: data structure for proxy caching logic
- parser_helper.h: parse http response returned from server and client http request 
