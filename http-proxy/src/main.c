#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

#include "proxy.h"
#include "epoll_mgr.h"

static void handle_sigint(int sig);


static void handle_sigint(int sig)
{
  (void)sig;
  
  printf("\nshutting proxy server down\n");
  
  proxy_cleanup();
  exit(0);
}

int main(int argc,char *argv[])
{
  int port;
  
  if(argc!=2)
  {
    fprintf(stderr,"usage : %s <port> \n",argv[0]);
    exit(1);
  }
  
  port=atoi(argv[1]);
  
  if(port<=0)
  {
    fprintf(stderr,"invalid port number\n");
    exit(1);
  }
  
  signal(SIGINT,handle_sigint);
  
  epoll_init();
  proxy_init(port);
  proxy_run();
  
  proxy_cleanup();
  epoll_cleanup();
  
  return 0;
}


