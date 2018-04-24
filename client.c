#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> //contains AI_PASSIVE?

int main(){//this is the client
    client("8820");
}

int client(char* port){
   
    struct addrinfo *myAddr, *hints, *r;
    myAddr = (struct addrinfo*)malloc(sizeof(struct sockaddr));
    hints = (struct addrinfo*)malloc(sizeof(struct sockaddr));
    hints -> ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints -> ai_socktype = SOCK_STREAM;
    hints -> ai_family = PF_INET;

    int indic = getaddrinfo("less.cs.rutgers.edu", port, hints, &myAddr);
    int cliSocket;
    for(r = myAddr; r; r -> ai_next){
        cliSocket = socket(r -> ai_family, r -> ai_socktype, r -> ai_protocol);
        
        //socket successfully created
        if(cliSocket != -1){
            printf("great success %d\n", cliSocket);

            break;
        }
    }

    int con = connect(cliSocket, r -> ai_addr, sizeof(r -> ai_addr)); 
    printf("my connect: %d", con);
    printf(
    //need sockaddr* for connect function. 
    //get addrinfo struct from func. how to convert?
    //sockaddr is a struct inside addrinfo: -> ai_addr (duh?)

    
}
