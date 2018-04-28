#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include "libnetfiles.h"
#include <fcntl.h>
#include "util.h"

int main(){
  int status;
  status = netserverinit(HOST);
  int h =  netopen("dir", O_RDONLY);
  printf("The fd we got back from the server:%d\n", h);

  char* buf = (char*)malloc(sizeof(char) * 10);
  int n = netread(h, buf, 9);
  printf("%s\n", buf);
  printf("%d\n", n);
  //netwrite();
  return 0;  
}
