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
#include <string.h>

int main(){
  int status;
  status = netserverinit(HOST, Unrestricted);
  int h =  netopen("dir", O_RDONLY);
  printf("client: The fd we got back from the server:%d\n", h);
  printf("client: netOpen has done its job!\n");

  //char* buf = (char*)malloc(sizeof(char) * 30);
  //int n = netread(h, buf, 25);

  char* dafuq = "what the flipping fuck\n";
  int m = netwrite(h, dafuq, strlen(dafuq));  
  printf("client: result of netwrite: %d\n", m);

  //int n = netclose(h);
  //printf("client: result of netclose: %d\n", n);
  return 0;  
}
