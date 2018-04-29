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
  printf("The fd we got back from the server:%d\n", h);

  printf("client: netOpen has done its job!\n");
  char* buf = (char*)malloc(sizeof(char) * 10);
  int n = netread(h, buf, 9);

  //  char* dafuq = "what the fuck\n";
  //int m = netwrite(h, dafuq, strlen(dafuq));  
  //printf("result of netwrite: %d\n", m);
  //netwrite();
  return 0;  
}
