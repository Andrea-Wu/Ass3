#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include "util.h"
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

typedef struct sNode{
    char* str;
    struct node* fds;
    struct sNode* next;
} sNode;

typedef struct node{
    int fd;
    int read; //is it reading
    int write; //is is writing
    char* filename;
    int aMode; //access mode
    struct node* next;
} node;

sNode** hashtable;  //global              


#define BACKLOG 5
typedef struct connection{ //idk
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
} connection;

void addFd( int fd, int mode, char* filename, Access aMode);
int myClose(int fd, int con);
int myRead(int fd, int con, int numBytes);
int myOpen(char* filename, Access access, int mode, int con);
int myWrite(int fd, int con, char* writeMe, int numBytes);
int server(char* port);
void *print(void *arg);

int main(){ //this is the server
    
    //create hash table 
    hashtable = (sNode**)malloc(sizeof(sNode*) * 100);  

    server(PORT);
    return 0;
}

int server(char* port){
    struct addrinfo *hints, *result, *r;
    connection *con;
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

    con = (connection*)malloc(sizeof(connection));
    for(;;){
        con -> addr_len = sizeof(struct sockaddr_storage);
        printf("waiting to accept\n");
        con -> fd = accept(mySocket, (struct sockaddr *) &con -> addr, &con ->addr_len);
      
        if(con -> fd == -1){
             printf("did not accept\n");
             continue;
        }

       // printf("accepted\n");
        printf("server.c: what\n");
       Message message;
       printf("server.c: 1\n");
       int didRead = readMessage(con -> fd, &message ); ///right?
        printf("2\n");
        printf(" msg str = %s\n", message);
       MessageType messType = message.message_type;
       if(messType == Open){
         printf("3\n");
         //call open
         myOpen(message.filename, message.client_access, message.mode, con -> fd);
         printf("4\n");
       }else if(messType == Read){
         printf("Server:I get it man, you wanna read\n");
         myRead(message.fd, con -> fd, message.buffer_len);
         printf("Server:Here is your shit.\n");         
       }else if(messType == Write){

       }else if(messType == Close){

       }else{
            printf("this broke\n");
       }

        printf("4\n");

/*
        rc = pthread_create(&tid, NULL, print, con);
        if(rc != 0){
            printf("unable to create thread\n");
            close(con -> fd);
        }

        pthread_detach(tid);
        */

        con = malloc(sizeof(connection));
    }

    return 0;
}


void * print(void * arg){ //note the void*, void * == thread function
    pthread_t tid = pthread_self(); //changes the server function?
    printf("print is happen\n");
    char host[100], port[10], buf[101];
    connection *c = (connection*) arg;
    int rc, nread, nwrite;

    rc = getnameinfo((struct sockaddr*)&c->addr, c -> addr_len, host, 100, port, 10, NI_NUMERICSERV);

    if(rc == -1){
        printf("err in print\n");
        close(c ->fd);
        return NULL;
    }

    printf("[%s:%s] connection\n", host, port);

    while((nread = read(c ->fd, buf, 100)) != 0){
        buf[nread] = '\0';
        printf("[%s:%s] read %d bytes |%s|\n", host, port, nread, buf);
        nwrite = write(c->fd, "Fuck you bitch fuck", strlen("Fuck you bitch fuck"));

    }

    printf("[%s:%s] got EOF\n", host, port);

    close(c -> fd);
    return NULL;

}

int myOpen(char* filename, Access access, int mode, int con){
    //returns an int that represents state of file

    //has to check whether exclusive, unrestricted, transaction
    //IMPORTANT: let 0 = exclusive
                    //1 = unrestricted
                    //2 = transaction

    int fd; //fd that will be opened

    //tmp should always exist

    /*
    int bucket = hashFunction(filename);
    sNode* tmp = hashtable[bucket];

    int filename_open = 0;
    while(tmp){
        if(strcmp(tmp -> str, filename) == 0){
            filename_open = 1;
            break;
        }

        tmp = tmp -> next;
    }
    

    //tmp should be node that contains filename
    if(access == Unrestricted){
        if(mode == O_RDONLY || mode == O_RDWR){
            //check file hash table for transaction mode
        }

        if(mode == O_WRONLY || mode == O_RDWR){
            //check hash table for transaction mode 

            //check hash table for exclusive mode && already writing
        }
        
    }else if(access == Exclusive){
        if(mode == O_RDONLY || mode == O_RDWR){
            
        }

        if(mode == O_WRONLY || mode == O_RDWR){
            //check transaction mode

            //
        }
        
    }else if(access == Transaction){
        if(fileName_open){
            //lack of permission code error
            return -1;
        }

        //set files
    }else{
        printf("error, undefined mode\n");
    }
*/
    fd = open(filename, mode);
    printf("a     %ld \n", filename);
    addFd(fd, mode, filename, access);
    printf("b\n");
    Message* message = (Message*)malloc(sizeof(Message));
    printf("c\n");
    message -> fd = 1 * fd;
    printf("d\n");
    int didWrite = writeMessage(con, *message);
    printf("e\n");

    if(didWrite){
    printf("f\n");
        //did not write
        printf("server did not write to socket\n");
        return -1;
    }


}

int hashFunction(char* str){
    printf("%ld\n", str);
    int strLen = strlen(str);
    printf("h1\n");
    int bucket = 0; 
    printf("h2\n");
    int i = 0;
    printf("h3\n");
    while(i < strLen){
        bucket = bucket + str[i];
        printf("h4 %d \n", i);
        i++;
    }
    
    printf("h5\n");
    bucket = bucket % 100;
    return bucket;
}

pthread_mutex_t lockA;

void addFd( int fd, int mode, char* filename, Access aMode){
    
    int bucket = hashFunction(filename);
    printf("ass1\n");

    //create node to insert
    node* newNode = (node*)malloc(sizeof(node));
    printf("ass2\n");
    newNode -> fd = fd;

    if(mode == O_RDONLY || mode == O_RDWR){
        newNode -> read = 1;
    
    }else{
        newNode -> read = 0;
    }

    printf("ass3\n");

    if(mode == O_WRONLY || mode == O_RDWR){
        newNode -> write = 1;
    }else{
        newNode -> write = 0;
    }

    newNode -> aMode = aMode;
    printf("ass4\n");
    pthread_mutex_lock(&lockA);

    //find sNode list corresp. to filename
    sNode* tmp = hashtable[bucket];
    printf("phantasmagoria\n");
    //iterate thru sNode list until string is found
    while(tmp){
            printf("fanta %s hehe\n", filename);
            

        if(strcmp(tmp -> str, filename) == 0){
            printf("fantasy\n");
            break;
        }
        
        tmp = tmp -> next;
    }
    printf("leguminous\n");
    if(!tmp){ //tmp is null, string does not exist yet in hash table
        tmp = (sNode*)malloc(sizeof(sNode));
        printf("pulmonary gland\n");
        tmp -> str = filename;

        //insert into hash, randomly...
        sNode* tmp2 = hashtable[bucket];
        hashtable[bucket] = tmp;
        tmp -> next = tmp2;
    }


    //fdList should not be null
    node* tmp3 = tmp -> fds;
    tmp -> fds = newNode;
    newNode -> next = tmp3;

    pthread_mutex_unlock(&lockA);
}

int myRead(int fd, int con, int numBytes){
    char* buffer = (char*)malloc(sizeof(char) * (numBytes + 1));
    int byte_read = read(fd, buffer, numBytes);
   
    Message* message = (Message*)malloc(sizeof(Message));
    message -> buffer = buffer;
    message -> buffer_len = byte_read;
    message -> filename_len = -1;
    if (byte_read == -1){
      message->message_type = Error;
      message->return_code = errno;
    }else{
      message->message_type = ReadResponse;
    }  


    if(writeMessage(con, *message)){
        //did not write
        printf("server did not write to client\n");
        return -1;
    }

    free(message);
    return 0;
}

int myWrite(int fd, int con, char* writeMe, int numBytes){

    int bytesWritten = write(fd, writeMe, numBytes); 

    if(bytesWritten == -1){
        printf("server did not write to file\n");
        return 0;
    }

    Message* message = (Message*)malloc(sizeof(Message));
    message -> bytes_written = bytesWritten;

    if(writeMessage(con, *message)){
        printf("server did not write to client\n");
        return -1;
    }

    free(message);
    return 0;
    
}

int myClose(int fd, int con){
    Message* message = (Message*)malloc(sizeof(Message));

    if(close(fd)){
        printf("server failed to close file\n");
        message -> return_code = -1;
        if(writeMessage(con, *message)){
            printf("server did not write to client\n");
            return -2;
        }
        return -1;
    }else{
        message -> return_code = 0;
        if(writeMessage(con, *message)){
            printf("server did not write to client\n");
            return -2;
        }

        //remove fd from linked list
    }
}








