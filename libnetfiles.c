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
  m.message_type = Open;
  m.mode = flags;
  m.client_access = access_mode;
  file_len = strlen(pathname);
  m.filename_len = file_len;
  m.buffer_len = -1;

  file_name = (char*)malloc(sizeof(char) * (file_len + 1));
  strcpy(file_name, pathname);
  m.filename = file_name;

    writeMessage(1, m);

  //send this shit and read that shit
  if (writeMessage(socket_fd, m) < 0 ){
    printf("libnetfiles: You done fucked, can't write!\n");
    return -1;
  }
  printf("libnetfiles: netOpen starts to read\n");
  if (readMessage(socket_fd, response)<0){
    printf("libnetfiles: Didn't read shit!\n");
    return -1;
  }

  if (response->fd <  0){
    printf("libnetfiles: Failed to open file\n");
    errno = response->return_code;
    return -1;
  }
   
  close(socket_fd);

//    printMsg(response);

  return response->fd*-1;
}

ssize_t netread(int fildes, void* buf, size_t nbyte){
    int socket_fd, file_len;
    char* file_name;
    Message m;
    Message *response = (Message*)malloc(sizeof(Message));

    //establish connection
    socket_fd = openCon();
    if(socket_fd <0){
      printf("libnetfiles.c: client connection to server failed\n");
    }else{
      printf("libnetfiles.c: client connected to server\n");
    }

    m.client_access = access_mode;
    m.message_type = Read;
    m.fd = fildes;
 // m.buffer = buf; //probably don't need this
    m.buffer_len = -1;
    m.filename_len = -1;
    m.bytes_written = nbyte; 

//i'm not sure how you want this, temp sending length we want written as bytes_written
//and recieveing bytes actually read witn bytes_written
    
    printf("libnetfiles.c: netRead, 107\n");
    if(writeMessage(socket_fd, m) < 0){
        printf("libnetfiles.c: you did not send mesg to server!\n");
        return -1;
    }
    printf("libnetfiles.c: successfully sent msg to server\n");
    if(readMessage(socket_fd, response) < 0){
        printf("libnetfiles.c: Didn't get response from server\n");
        return -1;
    }
    printf("libnetfiles.c: netread got response from server\n"); 
    printf("libnetfiles.c: netRead 118 number of bytes read: %d\n", response -> bytes_written);


    if(response -> buffer){
        printf("%s\n", response -> buffer); //response -> buffer is null, why?
    }else{
        printf("libmetfiles.c: did not read in string\n");
    }
    close(socket_fd);

    //printMsg(response);

    if (response->message_type == Error){
      errno =response->return_code;
    }
  return response->bytes_written;
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
    m.buffer = buffer; 
    m.buffer_len = nbyte;
    m.bytes_written = nbyte;
    m.filename_len = -1;

    printf("libnetfiles.c message data: nbyte=%d, buffer = %s\n", m.buffer_len, m.buffer);

    if(writeMessage(socket_fd, m) < 0){
        printf("libnetfiles.c: message failed to send to server!\n");
        return -1;
    }
    printf("libnetfiles.c: message sent to server\n");

    if(readMessage(socket_fd, response) < 0){
        printf("libnetfiles.c: no response from server!\n");
        return -1;
    }
    printf("libnetfiles.c: 158 number of bytes written: %d\n", response -> bytes_written);

    if(response -> buffer){
        printf("libnetfiles.c: reponse str %s\n", response -> buffer); //response -> buffer is null, why?
    }
    close(socket_fd);

    //printMsg(response);
    if (response->message_type == Error){
      errno =response->return_code;
    }

  return response->bytes_written;
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
    printf("libnetfiles.c: 184 YOU didn't write to server!\n");
    return -1;
  }
  if(readMessage(socket_fd, response) < 0){
    printf("libnetfiles.c: Didn't get response from server!\n");
    return -1;
  }
  close(socket_fd);
  printf("libnetfiles.c: Successfully closed server connection!\n");

  //printMsg(response);
  if (response->message_type == Error){
      errno =response->return_code;
  }
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
      printf("libnetfiles.c: Successfully created network socket: %d\n",socket_fd );
      break;
    }
  }
  //at this point socket has been created, ready to communicate

  connection = connect(socket_fd, res->ai_addr, sizeof(*(res->ai_addr)));

  if(connection == 0){
    printf("libnetfiles.c: Successfully connected\n");
    return socket_fd;
  }else{
   printf("libnetfiles.c: connection Failed\n");
    return -1;
  }
  
}





