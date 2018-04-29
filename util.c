#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

int writeMessage(int fd, Message m)
{
  int fd_copy = dup(fd);
  FILE * sock = fdopen(fd_copy, "w");

  if (sock ==NULL){
    printf("util.c: Socket not working\n");
    return -1;
  }
    printf("util.c: socket exists\n");
  if (fprintf(sock, "%d %d %d %d %d %d %d %d ", m.message_type, m.mode, m.client_access, m.fd, m.buffer_len, m.filename_len, m.return_code, m.bytes_written) < 0){
    printf("util.c: Message failed to send1\n");
    return -1;
  }
    printf("util.c: initial message sent\n");
  if(m.buffer_len > 0){
    printf("util.c: buffer_len is positive!\n");
    printf("util.c: currently sending %s as buffer\n", m.buffer); 
    if(fwrite(m.buffer,m.buffer_len+1,1,sock)<0){
      printf("util.c: Message failed to send2\n");
      return -1;
    }
    printf("util.c: successfully wrote %s as buffer\n", m.buffer);
  }

    
  if(m.filename_len > 0){ 
    printf("util.c: filename_len is positive!\n");
    printf("util.c: filename is %s\n", m.filename);
    if(fwrite(m.filename,m.filename_len + 1,1,sock)<0){
      printf("util.c: Message failed to send3\n");
      return -1;
    }
    printf("util.c: successfully wrote %s as filename\n", m.filename);
  }

  printf("util.c: end writeMessage\n");
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

  printf("util.c: socket works for reading\n");
  int what;
  if((what = fscanf(sock, "%d %d %d %d %d %d %d %d ", &m->message_type, &m->mode, &m->client_access, &m->fd, &m->buffer_len, &m->filename_len, &m->return_code, &m->bytes_written))<0){
    printf("util.c: Failed to receive message: a   %d \n", what);
    perror("util.c: myError");
    return -1;
  }
  printf("%d ",m->message_type);
  printf("%d ",m->mode);
  printf("%d ",m->client_access);
  printf("%d ",m->fd);
  printf("%d ",m->buffer_len);
  printf("%d ",m->filename_len);
  printf("%d ",m->return_code);
  printf("%d\n",m->bytes_written);
  printf("util.c: initial msg scanned, items scanned: %d\n", what);
  if(m->buffer_len > 0){

        printf("util.c: 61... scanning buffer. create an array of length %d\n", m->bytes_written + 1);
    m->buffer = (char*)malloc((m->buffer_len +1)* (sizeof(char))); 
    
    if(fread(m->buffer,m->buffer_len+1,1,sock)<0){
      printf("util.c: Failed to receive message: b\n");
      return -1;
    }
    printf("util.c: successfully scanned in %s as buffer\n", m->buffer);
  }
    if(m -> filename_len){
        printf("util.c: filename length is %d\n", m-> filename_len);
    }
  if(m->filename_len > 0){
    printf("util.c: 71 scanning filename\n");
    m->filename = (char*)malloc(((m->filename_len) + 100) * (sizeof(char)));
    if(fread(m->filename,m->filename_len+1,1,sock)<0){
      printf("Failed to receive message: c\n");
      return -1;
    }
    printf("util.c: successfully scanned %s as filename\n", m->filename);
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
