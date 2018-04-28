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
    printf("util.c: 15\n");
  if (fprintf(sock, "%d %d %d %d %d %d %d %d ", m.message_type, m.mode, m.client_access, m.fd, m.buffer_len, m.filename_len, m.return_code, m.bytes_written) < 0){
    printf("Message failed to send1\n");
    return -1;
  }
    printf("util.c: 20\n");
  if(m.buffer_len > 0){
    printf("util.c: buffer_len is positive!\n");
    printf("currently sending %s as buffer\n", m.buffer); 
    if(fprintf(sock, "%s\n", m.buffer)<0){
      printf("Message failed to send2\n");
      return -1;
    }
  }


  printf("util.c: 29\n");
  if(m.filename_len > 0){ 
    printf("util.c: filename_len is positive!\n");
    if(fprintf(sock, "%s\n", m.filename)<0){
      printf("Message failed to send3\n");
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
  int what;
  if(what = fscanf(sock, "%d %d %d %d %d %d %d %d", &m->message_type, &m->mode, &m->client_access, &m->fd, &m->buffer_len, &m->filename_len, &m->return_code, &m->bytes_written)<0){
    printf("Failed to receive message: a   %d \n", what);
    perror("myError:");
    return -1;
  } 
  printf("fuck\n");
  if(m->buffer_len > 0){
        printf("util.c: 59 scanning buffer\n");
        printf("util.c: 61... create an array of length %d\n", m->bytes_written + 1);
        printf("bitch\n");
    m->buffer = (char*)malloc((m->bytes_written +1)* (sizeof(char))); 
        printf("hell\n");
    if(fscanf(sock, "%s", m->buffer)<0){
        printf("ass\n");
      printf("Failed to receive message: b\n");
      return -1;
    }
  }
    printf("shit %d\n", m-> filename_len);
  if(m->filename_len > 0){
    printf("util.c: 71 scanning filename\n");
    printf("my god\n");
    m->filename = (char*)malloc(((m->filename_len) + 100) * (sizeof(char)));
    printf("bum\n"); 
    printf("omfg %ldiiiii\n", m->filename);
    if(fscanf(sock, "%s", m->filename)<0){
      printf("Failed to receive message: c\n");
      return -1;
    }
    printf("lol\n");
    printf("%s\n", m->filename);
  }
  fclose(sock);
  return 0;
}

/*
int main(){
    Message m;
    m.message_type = Open;
    m.mode = 3;
    m.client_access = Unrestricted;
    m.filename_len = 3;
    m.filename = "dir";
    m.buffer_len = -3;
    writeMessage(1,m);
    return 0;
}
*/
