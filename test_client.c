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
  netopen("dir", O_RDONLY);
  return 0;  
}
