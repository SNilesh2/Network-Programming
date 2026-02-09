#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "http.h"

static http_method_t parse_method(const char *buf);

int http_parse_request(const char *buf,http_request_t *req)
{
  memset(req,0,sizeof(*req));
  
  req->method=parse_method(buf);
  
  //CONNECT example.com:443 HTTP/1.1
  if(req->method==HTTP_CONNECT)
  {
    char hostport[256];
    
    sscanf(buf,"CONNECT %255s",hostport);
    
    char *colon=strchr(hostport,':');
    
    if(!colon)
    {
      return -1;
    }
    *colon='\0';
    
    strcpy(req->host,hostport);
    req->port=atoi(colon+1);
    
    return 0;
  }
  
  //GET http://example.com/index.html HTTP/1.1
  //HOST: example.com
  
  char *host_hdr;
  
  host_hdr=strstr(buf,"Host:");
  
  if(!host_hdr)
  {
    return -1;
  }
  
  sscanf(host_hdr,"Host: %1023s",req->host);
  
  req->port=80;
  
  char url[1024];
  
  sscanf(buf,"%*s %1023s",url);
  
  char *path=strstr(url,"://");
  
  if(path)
  {
    path=strchr(path+3,'/');
  }
  else
  {
    path=strchr(url,'/');
  }
  
  if(path)
  {
    strcpy(req->path,path);
  }
  else
  {
    strcpy(req->path,"/");
  }
  
  
  //content length
  
  req->content_length=0;
  
  char *content_len_hdr;
  
  content_len_hdr=strstr(buf,"Content-Length:");
  
  if(content_len_hdr)
  {
    sscanf(content_len_hdr,"Content-Length: %d",&req->content_length);
  }
  
  return 0;
}

static http_method_t parse_method(const char *buf)
{
  if(strncmp(buf,"GET ",4)==0)
  {
    return HTTP_GET;
  }
  
  if(strncmp(buf,"POST ",5)==0)
  {
    return HTTP_POST;
  }
  
  if(strncmp(buf,"CONNECT ",8)==0)
  {
    return HTTP_CONNECT;
  }
  
  return HTTP_UNKNOWN;
}


int http_build_server_request(const char *client_req,char *server_req, int server_req_size)
{
  const char *method_end;
  const char *url_start;
  const char *path;
  
  method_end=strchr(client_req,' ');
  
  if(!method_end)
  {
    return -1;
  }
  url_start=method_end+1;
  
  path=strstr(url_start,"://");
  
  if(path)
  {
    path=strchr(path+3,'/');
  }
  else
  {
    path=strchr(url_start,'/');
  }
  
  if(!path)
  {
   path="/";
  }
  
  int used;
  const char *line_end=strstr(client_req," HTTP/");
  
  if(!line_end)
  {
    return -1;
  }
  used=snprintf(server_req,server_req_size,"%.*s %.*s HTTP/1.1\r\n",(int)(method_end-client_req),client_req,(int)(line_end-path),path);
  
  if(used<=0 || used>=server_req_size)
  {
    return -1;
  }
  
  const char *h=strstr(client_req,"\r\n");
  
  if(!h)
  {
    return -1;
  }
  
  h+=2;
  
  while(*h)
  {
    const char *hdr_end=strstr(h,"\r\n");
    
    if(!hdr_end)
    {
      break;
    }
    
    if(hdr_end==h)
    {
      h+=2;
      break;
    }
    
    if(strncasecmp(h,"Proxy-Connection:",17)!=0 && strncasecmp(h,"Connection:",11)!=0)
    {
      int len=hdr_end-h+2;
      if((int)strlen(server_req)+len >= server_req_size)
      {
        return -1;
      }
      
      strncat(server_req,h,len);
    }
    
    h=hdr_end+2;
  }
  
  strncat(server_req,"Connection: close\r\n\r\n",server_req_size-strlen(server_req)-1);
  
  return 0;
}
