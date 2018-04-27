#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include "libnetfiles.h"
#include <fcntl.h>

int main(){
  int status;
  status = netserverinit("decorator.cs.rutgers.edu");
  printf("%d\n", status);
  int h =  netopen("dir", O_RDONLY);
  printf("%d\n", h);

    char* buf = (char*)malloc(sizeof(char) * 10);
  int n = netread(h, buf, 9);
    printf("%s\n", buf);
    printf("%d\n", n);
  //netwrite();
  return 0;  
}
