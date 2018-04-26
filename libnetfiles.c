#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libnetfiles.h"
#include <errno.h>

#define PORT "8820"

struct addrinfo* hostinfo;
int mode; //for ext A usage

int netserverinit(char* hostname, int fileMode){
  //fileMode follows numbers in assignment instructions

  struct addrinfo hints, serverinfo;
  int status;

  memset(&hints, 0 , sizeof(struct serverinfo));
  hints.ai_family = PF_INET;
  hints.ai_socketype = SOCK_STREAM;
   
  status = getaddrinfo(hostname, PORT, &hints, &serverinfo);

  if(status != 0){
    perror("getaddrinfo");
    printf("getaddrinfo %d\n", strerror(errno));
    printf("getaddrinfo : %s \n", gai_strerror(status));
    return -1;
  }

  //set mode flag depending on the mode
  hostinfo=serverinfo;
  freeaddrinfo(serverinfo);
  return 0; 
}

int netopen(const char* pathname, int flags){
   int socket_fd;
   //establish connection
   socket_fd = 
}

ssize_t netread(int fildes, void* buf, size_t nbyte){

}


ssize_t netwrite(int fildes, const void* buf, size_t nbyte){

}


int netclose(int fd){


}

int openCon(){
  int socket_fd,connection;
  struct addrinfo *res;

  for (res = hostinfo; res; res->ai_next){
    socket_fd = socket(res->ai_family, res->ai_socketype, res->ai_protocol);
    if (socket_fd == -1){
      perror("socket");
      return -1;
    }else{
      printf("Successfully created network socket: %d\n",socket_fd );
      break;
    }
  }
  //at this point socket has been created, ready to communicate

  connection = connect(socket_fd, res->ai_addr, sizeof(*(res->ai_addr)));

  if(connection == 0){
    printf("Successfully connected\n");
    return socket_fd;
  }else{
    printf("connection Failed\n");
    return -1;
  }
  
}
