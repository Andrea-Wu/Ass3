#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

int writeMessage(int fd, Message m)
{
  int fd_copy = dup(fd);
  FILE * sock = fdopen(fd_copy, "w");

  if (sock ==NULL){
    printf("Socket not working");
    return -1;
  }

  if (fprintf(sock, "%d %d %d %d %d %d ", m.message_type, m.mode, m.client_access, m.fd, m.buffer_len, m.filename_len) < 0){
    printf("Message failed to send");
    return -1;
  }

  if(m.buffer_len > 0){
    if(fprintf(sock, "%s ", m.buffer)<0){
      printf("Message failed to send");
      return -1;
    }
  }
  
  if(m.filename_len > 0){ 
    if(fprintf(sock, "%s ", m.filename)<0){
      printf("Message failed to send");
      return -1;
    }
  }
  fflush(sock);
  fclose(sock);
  return 0;
}

int readMessage(int fd, Message* m)
{
  int fd_copy = dup(fd);
  FILE * sock = fdopen(fd_copy, "r");
  if (sock ==NULL){
    printf("Socket not working");
    return -1;
  }
  if(fscanf(sock, "%d %d %d %d %d %d", &m->message_type, &m->mode, &m->client_access, &m->fd, &m->buffer_len, &m->filename_len)<0){
    printf("Failed to receive message");
    return -1;
  } 
  if(m->buffer_len > 0){
    m->buffer = (char*)malloc(m->buffer_len * (sizeof(char))); 
    if(fscanf(sock, "%s", m->buffer)<0){
      printf("Failed to receive message");
      return -1;
    }
  }
  if(m->filename_len > 0){
    m->filename = (char*)malloc(m->filename_len*(sizeof(char)));
    if(fscanf(sock, "%s", m->filename)<0){
      printf("Failed to receive message");
      return -1;
    }
  }
  fclose(sock);
  return 0;
} 
