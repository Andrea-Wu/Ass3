#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include "libnetfiles.h"

int main(){
  int status, connection;
  status = netserverinit("decorator.cs.rutgers.edu");
  printf("%d\n", status);
  connection = openCon();
  return 0;  
}
