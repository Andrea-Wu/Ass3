#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libnetfiles.h"
#include <errno.h>
#include "util.h"


struct addrinfo* hostinfo;
Access access_mode;

int netserverinit(char* hostname, int filemode){
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
  access_mode = filemode;
  hostinfo=serverinfo;
  //freeaddrinfo(serverinfo);
  //ask Aaron when he freed this linked list or how he managed to make a copy of it.
  return 0; 
}

int netopen(const char* pathname, int flags){
  int socket_fd,file_len;
  char* file_name;
  Message m;
  Message *response = (Message*)malloc(sizeof(Message));
  
  //establish connection
  socket_fd = openCon();
  
  //let's make this fucking message!
  m.client_access = access_mode;
  m.message_type = Open;
  m.mode = flags;
  m.client_access = access_mode;
  file_len = strlen(pathname);
  m.filename_len = file_len;
  m.buffer_len = -1;

  file_name = (char*)malloc(sizeof(char) * (file_len + 1));
  strcpy(file_name, pathname);
  m.filename = file_name;

  //send this shit and read that shit
  if (writeMessage(socket_fd, m) < 0 ){
    printf("You done fucked, can't write!");
    return -1;
  }
  if (readMessage(socket_fd, response)<0){
    printf("Didn't read shit!");
    return -1;
  }

  if (response->fd <  0){
    printf("Failed to open file");
    return -1;
  }
 
 //make fd negative
 response -> fd = (response -> fd) * -1;
  close(socket_fd);
  return response->fd;
}

ssize_t netread(int fildes, void* buf, size_t nbyte){
    int socket_fd, file_len;
    char* file_name;
    Message m;
    Message *response = (Message*)malloc(sizeof(Message));

    printf("read1\n");
    //establish connection
    socket_fd = openCon();

    m.client_access = access_mode;
    m.message_type = Read;
    m.fd = fildes;
 //   m.buffer = buf; //probably don't need this
    m.buffer_len = -1;
    m.filename_len = -1;
    m.bytes_written = nbyte; 

//i'm not sure how you want this, temp sending length we want written as bytes_written
//and recieveing bytes actually read witn bytes_written

    
    
    m.filename_len = -1;
    printf("libnetfiles.c: netRead, 107\n");
    if(writeMessage(socket_fd, m) < 0){
        printf("libnetfiles.c: 109 libnetfiles.c: 107 YOU didn't write!\n");
        return -1;
    }
    printf("libnetfiles.c: netRead, 112 \n");
    if(readMessage(socket_fd, response) < 0){
        printf("libnetfiles.c: 114 Didn't read!\n");
        return -1;
    }
    printf("read4\n"); 
    printf("libnetfiles.c: netRead 118 number of bytes read: %d\n", response -> bytes_written);


    if(response -> buffer){
        printf("%s\n", response -> buffer); //response -> buffer is null, why?
    }else{
        printf("your str dind't read\n");
    }
    close(socket_fd);
  return 0;
}


ssize_t netwrite(int fildes, const void* buf, size_t nbyte){
   int socket_fd, file_len;
    char* file_name;
    Message m;
    Message *response = (Message*)malloc(sizeof(Message));

    //copy??
    char* buffer = (char*)malloc(sizeof(char) * (nbyte +1));
    strcpy(buffer, buf);

    //establish connection
    socket_fd = openCon();

    m.client_access = access_mode;
    m.message_type = Write;
    m.fd = fildes;
    m.buffer = buffer; //this might break
    m.buffer_len = nbyte;
    m.filename_len = -1;

    //this might not be necessary
    //char* buffer = (char*)malloc(sizeof(char)*(nbyte + 1))
    
    m.filename_len = -1;
    if(writeMessage(socket_fd, m) < 0){
        printf("libnetfiles.c: 150 YOU didn't write!\n");
        return -1;
    }
    if(readMessage(socket_fd, response) < 0){
        printf("libnetfiles.c: 154 Didn't read!\n");
        return -1;
    }
    printf("libnetfiles.c: 157 read4\n"); 
    printf("libnetfiles.c: 158 number of bytes read: %d\n", response -> bytes_written);
    printf("libnetfiles.c: 161 string read  |%s |", response -> buffer);

    if(response -> buffer){
        printf("%s\n", response -> buffer); //response -> buffer is null, why?
    }
    close(socket_fd);
 
  return 0;
}


int netclose(int fd){
  int socket_fd, file_len;
  char* file_name;
  Message m;
  Message *response = (Message*)malloc(sizeof(Message));
  
  //establish connection
  socket_fd = openCon();
  
  m.client_access = access_mode; 
  m.message_type = Close;
  m.fd = fd;
  //m.buffer = buf; //probably don't need this
  m.buffer_len = -1;
  m.filename_len = -1;
  if(writeMessage(socket_fd, m) < 0){
    printf("libnetfiles.c: 184 YOU didn't write!\n");
    return -1;
  }
  if(readMessage(socket_fd, response) < 0){
    printf("Didn't read!\n");
    return -1;
  }
  close(socket_fd);
  printf("Successfully closed!\n");
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
