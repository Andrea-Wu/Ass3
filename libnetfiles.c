#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "libnetfiles.h"
#include <errno.h>


int netserverinit(char* hostname, int fileMode){
    //fileMode follows numbers in assignment instructions
  
}

int netopen(const char* pathname, int flags){

}

ssize_t netread(int fildes, void* buf, size_t nbyte){

}


ssize_t netwrite(int fildes, const void* buf, size_t nbyte){

}


int netclose(int fd){


}
