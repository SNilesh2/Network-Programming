#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include <ctype.h>

#include<openssl/sha.h>
#include<openssl/evp.h>

#include "websocket.h"

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

static char *strcasestr_custom(const char *haystack, const char *needle);

static void generate_accept_key(const char *client_key, char *accept_key);

//create new websocket client
websocket_client_t *ws_create(int fd)
{
  websocket_client_t *client=malloc(sizeof(websocket_client_t));
  
  if(!client)
  {
    return NULL;
  }
  
  client->fd=fd;
  client->state=WS_STATE_HTTP_HANDSHAKE;
  client->recv_len=0;
  client->send_len=0;
  client->has_set_username=0; //no name set
  memset(client->username,0,MAX_USERNAME_LEN);
  memset(client->recv_buffer,0,MAX_BUFFER);
  memset(client->send_buffer,0,MAX_BUFFER);
  
  return client;
}

//destroy client
void ws_destroy(websocket_client_t *client)
{
  if(!client)
  {
    return;
  }
  
  close(client->fd);
  free(client);
}

//perform http handshake server side
//the client mentioned here is the client after accept function(client_fd)
int ws_perform_handshake(websocket_client_t *client)
{

    // DEBUG: See exactly what the browser sent
    //printf("--- RECEIVED HEADER ---\n%s\n-----------------------\n", client->recv_buffer);

    char *key_label = strcasestr_custom(client->recv_buffer, "Sec-WebSocket-Key:");
    
    if (!key_label) 
    {
        return -1;
    }
    
    
  // 2. Move pointer past the label and skip ANY spaces
    char *key_start = key_label + strlen("Sec-WebSocket-Key:");
    while (*key_start == ' ') key_start++;

  
  char client_key[128];
  
  int i = 0;
  
  while (key_start[i] != '\r' && key_start[i] != '\n' && key_start[i] != ' ' && i < 127) {
        client_key[i] = key_start[i];
        i++;
    }
    client_key[i] = '\0';

    if (i == 0) {
        printf("Handshake Error: Extracted key is empty\n");
        return -1;
    }
  
  char accept_key[128];
  
  generate_accept_key(client_key,accept_key);
  
  char response[512];
  
  int len=snprintf(response,sizeof(response), 
      "HTTP/1.1 101 Switching Protocols\r\n"
      "Upgrade: websocket\r\n"
      "Connection: Upgrade\r\n"
      "Sec-WebSocket-Accept: %s\r\n\r\n", accept_key);
                 
  memcpy(client->send_buffer,response,len);
  client->send_len=len;
  
  memset(client->recv_buffer, 0, sizeof(client->recv_buffer));
  client->recv_len = 0;
  client->state=WS_STATE_CONNECTED;
  
  printf("Handshake complete for fd=%d\n",client->fd);
  
  return 0;
}


static char *strcasestr_custom(const char *haystack, const char *needle) {
    if (!*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        if (toupper(*haystack) == toupper(*needle)) {
            const char *h, *n;
            for (h = haystack, n = needle; *h && *n && toupper(*h) == toupper(*n); h++, n++);
            if (!*n) return (char *)haystack;
        }
    }
    return NULL;
}


//generate Sec-WebSocket-Accept key
static void generate_accept_key(const char *client_key,char *accept_key)
{
  char combined[256];
  unsigned char sha1_hash[SHA_DIGEST_LENGTH];
  
//combining client_key + GUID  
snprintf(combined,sizeof(combined),"%s%s",client_key,WS_GUID);
  
//SHA1 hash
  SHA1((unsigned char *)combined,strlen(combined),sha1_hash);
  
//base 64 
  EVP_EncodeBlock((unsigned char *)accept_key,sha1_hash,SHA_DIGEST_LENGTH);
  
}


//send text frame 
int ws_send_text(websocket_client_t *client,const char *msg,size_t len)
{
  if(len>65535)
  {
    return -1;
  }
  
  unsigned char frame[MAX_BUFFER];
  size_t offset=0;
  
  frame[offset++]=0x81; //10000001-> 0th bit 1 for FIN(message complete) ,last 4 bit 0001 for opcode (text frame)
  
  if(len<=125)
  {
    frame[offset++]=(unsigned char)len;
  }
  else
  {
    frame[offset++]=126;
    frame[offset++]=(len>>8) & 0xFF;
    frame[offset++]=len & 0xFF;
  }
  
  memcpy(frame+offset,msg,len);
  
  size_t frame_size=len+offset;
  
  if(client->send_len+frame_size>MAX_BUFFER)
  {
    return -1;
  }
  
  memcpy(client->send_buffer+client->send_len,frame,frame_size);
  
  client->send_len+=frame_size;
  
  return 0;
}

//close websocket
int ws_close(websocket_client_t *client)
{
  if(!client)
  {
    return -1;
  }
  
  client->state=WS_STATE_CLOSING;
  
  close(client->fd);
  return 0;
}


//handle read
int ws_handle_read(websocket_client_t *client)
{ 
  ssize_t n=recv(client->fd,client->recv_buffer+client->recv_len,MAX_BUFFER-client->recv_len,0);
  
  if(n==0)
  {
    printf("client disconnected fd=%d\n",client->fd);
    
    return -1;
  }
  
  if(n==-1)
  {
    if(errno==EAGAIN || errno==EWOULDBLOCK)
    {
      return 0;
    }
    perror("recv");
    return -1;
  }
  
  client->recv_len+=n;
  client->recv_buffer[client->recv_len]='\0';
  
  //handshake 
  if(client->state==WS_STATE_HTTP_HANDSHAKE)
  {
    if(strstr(client->recv_buffer,"\r\n\r\n"))
    {
      return ws_perform_handshake(client);
    }
    return 0;
  }
  //connected phase (receiving frames from clients)
  else if(client->state==WS_STATE_CONNECTED)
  {
    if(client->recv_len<2) return 0;
    
    unsigned char *buf=(unsigned char *)client->recv_buffer;
    
    size_t payload_len=buf[1] & 0x7F;
    size_t header_size=6;//2 headers + 4 mask
    
    if(payload_len==126)
    {
      if(client->recv_len<4) //need extended length
      {
        return 0;
      }
      payload_len=(buf[2]<<8) | buf[3];
      header_size=8;//extended: 2 headers + 2 ext len + 4 mask
    }
    
    if(client->recv_len < (payload_len + header_size))
    {
      return 0;
    }
    
    unsigned char *mask=buf + (header_size-4);
    
    unsigned char *payload=buf+header_size;
    
    //unmask the data
    for(size_t i=0;i<payload_len;i++)
    {
      payload[i]=payload[i]^mask[i%4]; 
    }
    
    payload[payload_len]='\0';
    
    printf("Received: %s from fd=%d\n",payload,client->fd);
    
    memmove(client->recv_buffer,payload,payload_len+1);
    client->recv_len=payload_len;
    return 1;//signal for full message is ready
  }
  return 0;
}


//handle write (buffered write)
int ws_handle_write(websocket_client_t *client)
{
  if(client->send_len==0)
  {
    return 0;
  }
  
  ssize_t n=send(client->fd,client->send_buffer,client->send_len,0);
  
  if(n==-1)
  {
    if(errno==EAGAIN || errno==EWOULDBLOCK)
    {
      return 0;
    }
    perror("send");
    return -1;
  }
  
  if((size_t)n<client->send_len)
  {
    memmove(client->send_buffer,client->send_buffer+n,client->send_len-n);
    
    client->send_len-=n;
  }
  else
  {
    client->send_len=0;
  }
  
  return 0;
}
