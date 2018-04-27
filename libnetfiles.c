#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libnetfiles.h"
#include <errno.h>
#include "util.h"

#define PORT "8820"

struct addrinfo* hostinfo;
Access access_mode;

int netserverinit(char* hostname){
  //fileMode follows numbers in assignment instructions

  struct addrinfo hints, *serverinfo;
  int status;

  memset(&hints, 0 , sizeof(struct addrinfo));
  hints.ai_family = PF_INET;
  hints.ai_socktype = SOCK_STREAM;
   
  status = getaddrinfo(hostname, PORT, &hints, &serverinfo);

  if(status != 0){
    perror("getaddrinfo");
    printf("getaddrinfo %s \n", strerror(errno));
    printf("getaddrinfo : %s \n", gai_strerror(status));
    return -1;
  }

  //set mode flag depending on the mode
  hostinfo=serverinfo;
  //freeaddrinfo(serverinfo);
  //ask Aaron when he freed this linked list or how he managed to make a copy of it.
  return 0; 
}

int netopen(const char* pathname, int flags){
  int socket_fd,file_len;
  char* file_name;
  Message m;
  Message *response;
  response = (Message*)malloc(sizeof(Message));
  
  //establish connection
  socket_fd = openCon();
  
  //let's make this fucking message!
  m.message_type = Open;
  m.mode = flags;
  m.client_access = access_mode;
  file_len = strlen(pathname);
  m.filename_len = file_len;
  m.buffer_len = -1;

  file_name = (char*)malloc(sizeof(char) * (file_len + 1));
  strcpy(file_name, pathname);
  m.filename = file_name;
  printf("%s\n", m.filename);

  //send this shit and read that shit
  if (writeMessage(socket_fd, m) < 0 ){
    printf("You done fucked, can't write!");
    return -1;
  }
  if (readMessage(socket_fd, response)<0){
    printf("Didn't read shit!");
    return -1;
  }

  if (response->fd < 0){
    printf("Failed to open file");
    return -1;
  }
  
  close(socket_fd);
  return response->fd;
}

ssize_t netread(int fildes, void* buf, size_t nbyte){
  return 0;
}


ssize_t netwrite(int fildes, const void* buf, size_t nbyte){
  return 0;
}


int netclose(int fd){

  return 0;
}

int openCon(){
  int socket_fd,connection;
  struct addrinfo *res;
  for (res = hostinfo; res; res = res->ai_next){
    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
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
