#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

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
struct connection{ //idk
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

int myClose(int fd, int con);
int myRead(int fd, int con, int numBytes);
int myOpen(char* filename, Access access, int mode, int con);
int myWrite(int fd, int con, char* writeMe, int numBytes);
int server(char* port);
void *print(void *arg);

int main(){ //this is the server
    
    //create hash table 
    hashtable = (sNode**)malloc(sizeof(sNode*) * 100);  

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
             printf("did not accept\n");
             continue;
        }

        printf("accepted\n");

       Message mess;
       int didRead = readMessage(con -> fd, &mess ); ///right?

       MessageType messType = mess.message_type;
       if(messType == Open){
            //call open
            myOpen(m -> filename, m-> client_access, m-> mode, con -> fd);
       }else if(messType == Read){
            
       }else if(messType == Write){

       }else if(messType = Close){

       }else{
            printf("this broke\n");
       }

    

/*
        rc = pthread_create(&tid, NULL, print, con);
        if(rc != 0){
            printf("unable to create thread\n");
            close(con -> fd);
        }

        pthread_detach(tid);
        */

        con = malloc(sizeof(struct connection));
    }

    return 0;
}


void * print(void * arg){ //note the void*, void * == thread function
    pthread_t tid = pthread_self(); //changes the server function?
    printf("print is happen\n");
    char host[100], port[10], buf[101];
    struct connection *c = (struct connection*) arg;
    int rc, nread, nwrite;

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

    sNode*  
   
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

    }else{
        printf("error, undefined mode\n");
    }

    fd = open(filename, mode);

    addFd(fd, mode, filename, access);

    Message* message = (Message*)malloc(sizeof(Message));
    message -> fd = fd;
    int didWrite = writeMessage(con, &message);

    if(didWrite){
        //did not write
        printf("server did not write to socket\n");
        return -1;
    }


}

int hashFunction(char* str){
    int strLen = strlen(str);

    int bucket = 0; 

    int i = 0;
    while(i < strLen){
        bucket = bucket + str[i];
        i++;
    }

    bucket = bucket % 100;
    return bucket;
}

pthread_mutex_t lockA;

void addFd( int fd, int mode, char* filename, Access aMode){
    
    int bucket = hashFunction(filename);

    //create node to insert
    node* newNode = (node*)malloc(sizeof(node));
    newNode -> fd = fd;

    if(mode == O_RDONLY || mode == O_RDWR){
        newNode -> read = 1;
    }else{
        newNode -> read = 0;
    }
    if(node == O_WRONLY || mode == O_RDWR){
        newNode -> write = 1;
    }else{
        newNode -> write = 0;
    }

    newNode -> aMode = aMode;

    pthread_mutex_lock(&lockA);

    //find sNode list corresp. to filename
    sNode* tmp = hashtable[bucket];

    //iterate thru sNode list until string is found
    while(tmp){
        if(strcmp(tmp -> str, filename) == 0){
            break;
        }
        
        tmp = tmp -> next;
    }

    if(!tmp){ //tmp is null, string does not exist yet in hash table
        tmp = (sNode*)malloc(sizeof(sNode));

        //insert into hash, randomly...
        sNode* tmp2 = hashtable[bucket];
        hashtable[bucket] = tmp;
        tmp -> next = tmp2;
    }

    node* fdList = tmp -> fds;

    //fdList should not be null
    node* tmp3 = tmp -> fdList;
    tmp -> fds = newNode;
    newNode -> next = tmp;

    pthread_mutex_unlock(&lockA);
}

int myRead(int fd, int con, int numBytes){
    char* buffer = (char*)malloc(sizeof(char) * (numBytes + 1));
    read(fd, buffer, numBytes);

    Message* message = (Message*)malloc(sizeof(Message));
    message -> buffer = buffer;
    message -> buffer_len = numBytes +1;


    if(writeMessage(con, &message)){
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
        return;
    }

    Message* message = (Message*)malloc(sizeof(Message));
    message -> bytes_written = bytesWritten;

    if(writeMessage(con, &message)){
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
        messsage -> return_code = -1;
        if(writeMessage(con, message)){
            printf("server did not write to client\n");
            return -2;
        }
        return -1;
    }else{
        message -> return_code = 0;
        if(writeMessage(con, message)){
            printf("server did not write to client\n");
            return -2;
        }

        //remove fd from linked list
    }
}








