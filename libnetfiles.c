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

  struct addrinfo hints;
  int status;

  memset(&hints, 0 , sizeof(struct addrinfo));
  hints.ai_family = PF_INET;
  hints.ai_socketype = SOCK_STREAM;
   
  status = getaddrinfo(hostname, PORT, &hints, &hostinfo);

  if(status != 0){
    perror("getaddrinfo");
    printf("getaddrinfo %d\n", strerror(errno));
    printf("getaddrinfo : %s \n", gai_strerror(status));
    return -1;
  }

  //set mode flag depending on the mode
  
  freeaddrinfo(serverinfo);
  return 0; 
}

int netopen(const char* pathname, int flags){
   
}

ssize_t netread(int fildes, void* buf, size_t nbyte){

}


ssize_t netwrite(int fildes, const void* buf, size_t nbyte){

}


int netclose(int fd){


}

int openCon(){
  
}
