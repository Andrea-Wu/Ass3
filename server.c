#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

#define BACKLOG 5
struct connection{ //idk
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

int server(char* port);
void *print(void *arg);

int main(){ //this is the server
    server("8820");
    return 0;
}

int server(char* port){
    struct addrinfo *hints, *result, *r;
    struct connection *con;
    int rc, mySocket;
    pthread_t tid;

    //initialize hints
    hints = (struct addrinfo* )malloc(sizeof(struct addrinfo));
    memset(hints, 0, sizeof(struct addrinfo));
    hints -> ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints -> ai_socktype = SOCK_STREAM;

    //get socket and address info for listening port
    rc = getaddrinfo(NULL, port, hints, &result);

    if(rc != 0){
        printf("error1");
        return -1;
    }

    //attempt to create socket
    for(r = result; r != NULL; r = r -> ai_next){
        mySocket = socket(r -> ai_family, r-> ai_socktype, r -> ai_protocol);
        if(mySocket == -1){
            continue;
        }

        if(!bind(mySocket, r -> ai_addr, r -> ai_addrlen) && !listen(mySocket, BACKLOG)){
            printf("got something\n");
            break;
        }

        //try next addrinfo
        close(mySocket);
    }

    freeaddrinfo(result);

    if(!r){ //no successful socket bind
        printf("error2");
        return -1;
    }

    //mySocket is bound & listening
    printf("Wainting for connection\n");

    con = malloc(sizeof(struct connection));
    for(;;){
        con -> addr_len = sizeof(struct sockaddr_storage);
        printf("waiting to accept\n");
        con -> fd = accept(mySocket, (struct sockaddr *) &con -> addr, &con ->addr_len);
      
        if(con -> fd == -1){
             printf("accept\n");
             continue;
        }

        printf("accepted\n");

        rc = pthread_create(&tid, NULL, print, con);
        if(rc != 0){
            printf("unable to create thread\n");
            close(con -> fd);
        }

        pthread_detach(tid);

        con = malloc(sizeof(struct connection));
    }

    return 0;
}


void * print(void * arg){ //note the void*, void * == thread function
    pthread_t tid = pthread_self(); //changes the server function?
    printf("print is happen\n");
    char host[100], port[10], buf[101];
    struct connection *c = (struct connection*) arg;
    int rc, nread;

    rc = getnameinfo((struct sockaddr*)&c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);

    if(rc == -1){
        printf("err in print\n");
        close(c ->fd);
        return NULL;
    }

    printf("[%s:%s] connection\n", host, port);

    while((nread = read(c ->fd, buf, 100)) != 0){
        buf[nread] = '\0';
        printf("[%s:%s] read %d bytes |%s|\n", host, port, nread, buf);

    }

    printf("[%s:%s] got EOF\n", host, port);

    close(c -> fd);
    return NULL;

}









































