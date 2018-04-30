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
    int client_access; //access mode
    int numReads;
    int numWrites;
    struct node* next;
} node;

typedef struct m_and_c{
    Message* message;
    int conFd;
} m_and_c;

sNode** hashtable;  //global hash on filenames    
node** hashtable_fd; //global hash on fds



#define BACKLOG 5
typedef struct connection{ //idk
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
} connection;

void addFd( int fd, int mode, char* filename, Access client_access);
void* myClose(void* args);
void* myRead(void* args);
void* myOpen(void* args);
void* myWrite(void* args);
int server(char* port);
void *print(void *arg);
int hashFunction(char* str);
void writeErrMsg(int err, int con);

int main(){ //this is the server
    
    //create hash table that hashes on file names
    hashtable = (sNode**)malloc(sizeof(sNode*) * 100);  

    //create hash table that hashes on file descriptors
    //no chaining, for now

    hashtable_fd = (node**)malloc(sizeof(node*) * 1000);
    //assume file descriptors don't exceed 1000?

    FILE*  pidf = fopen("pid.txt", "w");
    fprintf(pidf,"my pid: %d\n", getpid());
    fclose(pidf);

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
        printf("error: kill server process\n");
        return -1;
    }

    //mySocket is bound & listening
    printf("server.c: Wainting for connection\n");

    con = (connection*)malloc(sizeof(connection));
    for(;;){
        con -> addr_len = sizeof(struct sockaddr_storage);
        printf("server.c: waiting to accept\n");
        con -> fd = accept(mySocket, (struct sockaddr *) &con -> addr, &con ->addr_len);
        printf("connection fd is %d\n", con->fd);
      
        if(con -> fd == -1){
             printf("server.c: connection from client failed\n");
             continue;
        }else{
            printf("server.c: connection from client successful\n");
        }

       // printf("accepted\n");

       //ASSUMPTION: this has to be malloced so that threading doesn't break
       Message*  message = (Message*)malloc(sizeof(Message));
       if(readMessage(con -> fd, message)){
          printf("server.c: initial msg from client not read\n");  
       }else{
            printf("server.c: initial message from client read\n");
            printMsg(message);
        }
        
        //IMPORTANT: this thread might break if not malloced
        pthread_t thread;
        m_and_c* m = (m_and_c*)malloc(sizeof(m_and_c));
        m -> message = message;
        m -> conFd = con -> fd; 

         MessageType messType = message -> message_type;
        if(messType == Open){
            if(message -> filename){
                printf("server.c: client wants to Open %s\n", message -> filename);
            }else{
                    printf("filename null\n");
            }
            //call open
           pthread_create(&thread, NULL, myOpen, (void*)m);

       }else if(messType == Read){
            printf("server.c: client wants to read\n");

            pthread_create(&thread, NULL, myRead, (void*)m);

       }else if(messType == Write){
            printf("server.c: client wants to write\n");

            pthread_create(&thread, NULL, myWrite, (void*)m);


       }else if(messType == Close){
            printf("server.c: client wants to close\n");

            pthread_create(&thread, NULL, myClose, (void*)m);


       }else{
            printf("this broke\n");
       }

       pthread_detach(thread);

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
    printf("server.c: 156 print is happen\n");
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


pthread_mutex_t lockB;
//int myOpen(char* filename, Access access, int mode, int con){
void* myOpen(void* args){
    //returns an int that represents state of file

    //has to check whether exclusive, unrestricted, transaction
    //IMPORTANT: let 0 = exclusive
                    //1 = unrestricted
                    //2 = transaction


    m_and_c * mc = (m_and_c*)args;
    Message* message_arg = mc -> message;
    int con = mc -> conFd;
    char* filename = message_arg -> filename;
    Access access = message_arg -> client_access;
    int mode = message_arg -> mode;


    Message* message = (Message*)malloc(sizeof(Message));
 

    pthread_mutex_lock(&lockB);
    int fd; //fd that will be opened

    //tmp should always exist

    
    int bucket = hashFunction(filename);
    sNode* tmp = hashtable[bucket];

    if(!tmp){
        //there are no sNodes in Linked list


        printf("server.c: 210 ya probably not fucked\n");
    }

    int filename_open = 0;
    while(tmp){
        if(strcmp(tmp -> str, filename) == 0){
            filename_open = 1;
            break;
        }

        tmp = tmp -> next;
    }
    

    //tmp should be node of LL that contains filename
    //fdnNode should not be null
    if(tmp){ 
        node* fdNode = tmp -> fds; 

        if(!fdNode){
            printf("server.c: 230 ya fucked\n");
        }

        //iterate thru fdNode to look for data...

        //i set default as false, but that might cause problems
        
        //look thru allll fds to see if any are on transaction mode
        //logically  if there's more than one fd then no fds are in transaction mode
        int itr = 0;
        int file_already_in_transaction_mode = 0;
        int file_is_opened_for_writing = 0;
        int file_is_opened_for_writing_in_exclusive = 0;
        node* temp = fdNode;
        while(temp){
            if(temp -> write == 1){
                file_is_opened_for_writing = 1;
            }

            if(temp -> write == 1 && temp -> client_access == Exclusive){
                file_is_opened_for_writing_in_exclusive = 1;
            }
            
            if(temp -> client_access == Transaction){
                file_already_in_transaction_mode = 1;
            }
            temp  = temp -> next;
        }

    
        if(access == Unrestricted){
            if(file_already_in_transaction_mode){
                writeErrMsg(LACK_OF_PERMISSION_ERROR, con);
                return;
            }

            if(mode == O_RDONLY || mode == O_RDWR){
                //it's lit
            }

            if(mode == O_WRONLY || mode == O_RDWR){
                if(file_is_opened_for_writing_in_exclusive){
                    writeErrMsg(LACK_OF_PERMISSION_ERROR, con);
                    return;
                }
            }
        
        }else if(access == Exclusive){
            if(file_already_in_transaction_mode){
                writeErrMsg(LACK_OF_PERMISSION_ERROR, con);
                return;
            }

            if(mode == O_RDONLY || mode == O_RDWR){
                //it's lit
            }

            if(mode == O_WRONLY || mode == O_RDWR){

                if(file_is_opened_for_writing){
                    writeErrMsg(LACK_OF_PERMISSION_ERROR, con);
                    return;
                }
            }
        
        }else if(access == Transaction){
            if(filename_open){
                //lack of permission code error
                writeErrMsg(LACK_OF_PERMISSION_ERROR, con);
                return;
            }

            //filename is not open at all
            //if it's not open, then you can do anything, really!

        }else{
            writeErrMsg(INVALID_FILE_MODE, con);
            return;
        }
    }
     pthread_mutex_unlock(&lockB);

    printf("about to open %s\n", filename);

    if( (fd  = open(filename, mode)) == -1){
        printf("server.c: file did not open\n");
        perror("error: ");
        message -> message_type = Error;
        message -> return_code = errno;
    }else{
        message -> message_type = OpenResponse;
        message -> fd = fd;
    }
    printf("about to addfd with fd %d\n", fd);
    addFd(fd, mode, filename, access);

    message -> filename_len = -1;
    message -> buffer_len = -1;

    //message -> filename = (char*)malloc(sizeof(char)* 4); //3?
    //strcpy(message -> filename, "dir");
    

    //message -> buffer_len = -1;
    //message -> filename_len = 3;

    writeMessage(1, *message);

    printf("connection fd is %d\n", con);
    int didWrite = writeMessage(con, *message);


    if(didWrite){
        //did not write
        printf("server did not write to client\n");
        return;
    }
    close(con);
}

void writeErrMsg(int err, int con){

    Message* message = (Message*)malloc(sizeof(Message));
    message -> message_type = Error;
    message -> return_code = err;
    writeMessage(con, *message);
    return;

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
pthread_mutex_t lockC;

void addFd( int fd, int mode, char* filename, Access client_access){ 
    printf("fd is %d\n", fd);

    if(fd == -1){
        printf("this shouldn't happen\n");
    }

    int bucket = hashFunction(filename);

    //create node to insert
    node* newNode = (node*)malloc(sizeof(node));
    newNode -> fd = fd;
    newNode -> filename = filename;

    //this is stupid 

    if(mode == O_RDONLY || mode == O_RDWR){
        newNode -> read = 1;
    
    }else{
        newNode -> read = 0;
    }


    if(mode == O_WRONLY || mode == O_RDWR){
        newNode -> write = 1;
    }else{
        newNode -> write = 0;
    }

    newNode -> client_access = client_access;

    //MUTEX: insert into hashtable
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
        tmp -> str = filename;

        //insert into hash, randomly...
        sNode* tmp2 = hashtable[bucket];
        hashtable[bucket] = tmp;
        tmp -> next = tmp2;
    }

    
    //ASSUMPTION: fdList should not be null
    node* tmp3 = tmp -> fds;
    tmp -> fds = newNode;
    newNode -> next = tmp3;

    pthread_mutex_unlock(&lockA);

    printf("server.c: inserted into hashtable\n");
    //MUTEX: insert into hashtable_fd
    pthread_mutex_lock(&lockC);
    
    //IMPORTANT: the assumption that file descriptors do not exceed 1000

    //IMPLEMENTATION: instead of storing an actual node in hash table, 
    //just store a pointer to the node that's in the other hastable
        //POTENTIAL PROBLEMS: ???
        //BENEFITS: don't need to free as many nodes
    printf("server.c: fd is %d\n", fd);
    hashtable_fd[fd] = newNode;


    pthread_mutex_unlock(&lockC);

}

//int myRead(int fd, int con, int numBytes){
void* myRead(void* args){

    //unpacking arguments
    m_and_c* mc = (m_and_c*)args;
    int con = mc -> conFd;
    Message* message_arg = mc-> message;
    int fd = message_arg -> fd;
    int numBytes = message_arg -> bytes_written;
    
    //make fd positive
    fd = fd * -1;

    Message* message = (Message*)malloc(sizeof(Message));
    message -> filename_len = -1;

    //IMPLEMENTATION: check hashtable_fd for file descriptor permissions
    node* fd_node = hashtable_fd[fd];
    if(fd_node -> read == 0){
        printf("server.c: fd does not have read permission\n");

        message -> buffer_len = -1;
        message -> message_type = Error;
        message -> return_code = errno;

        if(writeMessage(con, *message)){
            printf("server.c: permission error AND msg to client failed\n");
        }else{
            printf("server.c: permission failed err sent to client\n");
        }
        return;   
    }


    printf("server: 359    fd is %d, numbytes is %d\n", fd, numBytes);
    char* buffer = (char*)malloc(sizeof(char) * (numBytes + 1));
    int bytesRead = read(fd, buffer, numBytes);

    if(buffer){
        printf("server: 362\n");
        printf("server.c: 361 %d bytes read, %s\n", bytesRead, buffer);
    }

    if (bytesRead == -1){
      message->message_type = Error;
      message->return_code = errno;
      return;
    }else{
      message->message_type = ReadResponse;
      message -> buffer = buffer;
      message -> buffer_len = bytesRead;
      message -> bytes_written = bytesRead;
    }  

    if(writeMessage(con, *message)){
        //did not write
        printf("server did not write to client\n");
        return;
    }
    close(con);
    free(message);
}

//int myWrite(int fd, int con, char* writeMe, int numBytes){
void* myWrite(void* args){

    //unpacking arguments
    m_and_c* mc = (m_and_c*)args;
    int con = mc -> conFd;
    Message* message_arg = mc -> message;
    int fd = message_arg -> fd;
    char* writeMe = message_arg -> buffer;
    int numBytes = strlen(writeMe);


    //make fd positive
    fd = fd * -1;

    Message* message = (Message*)malloc(sizeof(Message));


    //IMPLEMENTATION: check hashtable_fd for file descriptor permissions
    node* fd_node = hashtable_fd[fd];
    if(fd_node -> write == 0){
        printf("server.c: fd does not have write permissions\n");
        writeErrMsg(errno, con);
        return;
    }

    printf("server.c: 466  fd= %d, str= %s, numBytes= %d\n", fd, writeMe, numBytes);
    int bytesWritten = write(fd, writeMe, numBytes); 

    if(bytesWritten == -1){
      printf("server did not write to fd %d\n", fd);
      perror("err");
      message->message_type = Error;
      message->return_code = errno;
        // return -1;
    }else{
      message->message_type = WriteResponse;
      message->buffer_len = -1;
      message->bytes_written = bytesWritten;
    }
    

    if(writeMessage(con, *message)){
        printf("server did not write to client\n");
        return;
    }
    
    close(con);
    free(message);
}

//int myClose(int fd, int con){

pthread_mutex_t lockD;
void* myClose(void* args){

    //unpacking args
    m_and_c* mc = (m_and_c*)args;
    int con = mc -> conFd;
    Message* message_arg = mc -> message;
    int fd = message_arg -> fd;

    Message* message = (Message*)malloc(sizeof(Message));
   
    fd = fd*-1;

      if(close(fd)){
              printf("server failed to close file\n");
            message -> message_type = Error;
            message -> return_code = errno;
            writeErrMsg(errno, con);
            return;
    }else{
            message->message_type = CloseResponse;
            message->buffer_len = -1;
            message->return_code = 0;
    }


    pthread_mutex_lock(&lockD);

    node* fdNode = hashtable_fd[fd];
    if(!fdNode){ //fd doesn't exist
        printf("not supposed to happen\n");
    }else{ //remove fd from LL
        char* fileString = fdNode -> filename; //this might break
        int bucket = hashFunction(fileString);

        sNode* sNodes = hashtable[bucket];

        //err if fd is not valid
        
    
        while(strcmp(sNodes -> str, fileString) != 0){
            sNodes = sNodes -> next;
        }
        
        //got the right sNode

        //ASSUMPTION: fdNodes will not be null
        node* fdNodes = sNodes -> fds;

        if(fdNodes -> next){ //fd isn't the only one open for given file

            if((fdNodes -> fd) == fd){ //the fd you want is the first in list
                sNodes -> fds = fdNodes -> next;
                free(fdNodes);
            }else{ //if not, iterate thru list
                while(((fdNodes -> next) -> fd) != fd){
                    fdNodes = fdNodes -> next;
                }
            
                //linked list removal
                node* temp  = fdNodes -> next;
                fdNodes -> next = (fdNodes -> next) -> next;
                free(temp);
            }
        }else{ //fd is the only one open for given file
            free(fdNodes);
            free(sNodes);
        }

        //did you free the ptr in the hashtable_fd?
        //just to be safe. 
        hashtable_fd[fd] = NULL;
    }
    
    //err if file's not even open to begin with
    pthread_mutex_unlock(&lockD);
    

    if(close(fd)){
        printf("server failed to close file\n");
        message -> message_type = Error;
        message -> return_code = errno;
    }else{
        message->message_type = CloseResponse;
        message->buffer_len = -1;
        message->return_code = 0;

    }

    if(writeMessage(con, *message)){
        printf("server did not write to client\n");
        return;
    }

    close(con);
    free(message);
    return 0;


    //remove fd from linked list
}








