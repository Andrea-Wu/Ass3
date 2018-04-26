#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> //contains AI_PASSIVE?
#include <errno.h>
#include <netinet/in.h>
#include "libnetfiles.h"

int main(){//this is the client
    client("8820");
}

int client(char* port){
   
    struct addrinfo *myAddr, hints, *r;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_INET;

    int indic = getaddrinfo("decorator.cs.rutgers.edu", port, &hints, &myAddr);
    
    if (indic != 0){
      perror("getaddrinfo");
      printf("getaddrinfo %d\n", strerror(errno)); 
      printf("getaddrinfo : %s \n", gai_strerror(indic));
      return -1;
    }       

    int cliSocket;
    for(r = myAddr; r; r -> ai_next){
      cliSocket = socket(myAddr -> ai_family, myAddr -> ai_socktype, myAddr -> ai_protocol);
      if (cliSocket < 0){
        perror("socket");
        return -1;
      } 
  
        //socket successfully created      
      if(cliSocket != -1){
            printf("Success! Socket_fd: %d\n", cliSocket);

            break;
        }
    }

    char host[256];
    getnameinfo(myAddr->ai_addr, myAddr->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
    puts(host);
 
    int con = connect(cliSocket, r -> ai_addr, sizeof(*(r -> ai_addr))); 
    printf("my connect: %d\n", con);
    printf("%d\n", sizeof(*(r -> ai_addr)));
    perror("wtf??:");
    //need sockaddr* for connect function. 
    //get addrinfo struct from func. how to convert?
    //sockaddr is a struct inside addrinfo: -> ai_addr (duh?)
    
                
}
