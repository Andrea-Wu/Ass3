#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <socket.h>

int main(){ //this is the server

    //get address info to use in bind()
    //getaddrinfo
    struct addrinfo* result;
    struct addrinfo* hints;
    
    memset(&hints, 0, sizeof(addrInfo


    getaddrinfo(NULL,"13131",, ); //(char* node, char* service, struct addrinfo, ** res  )
//will run on port 13131

    //create socket
   int mySocket = socket(PF_INET, SOCK_STREAM, 0);

    //bind socket
   int myBind = (mySocket, addr, addr_len);

}
