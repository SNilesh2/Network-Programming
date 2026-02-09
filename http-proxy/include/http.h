#ifndef HTTP_H
#define HTTP_H

typedef enum
{
  HTTP_GET,
  HTTP_POST,
  HTTP_CONNECT,
  HTTP_UNKNOWN
}http_method_t;

typedef struct
{
  http_method_t method;
  char host[256];
  int port;
  char path[1024];
  int content_length;
}http_request_t;


//parse http request from buffer
int http_parse_request(const char *buf,http_request_t *req);

//rewrite buffer to server format
int http_build_server_request(
        const char *client_req,
        char *server_req,
        int server_req_size
  );
  
  #endif
