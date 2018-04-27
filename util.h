#define UTIL_H_

typedef enum {Unrestricted, Exclusive, Transaction} Access;
typedef enum {Open, Read, Write, Close, Error, Success, ReadResponse, OpenResponse} MessageType;

typedef struct
{
  MessageType message_type;
  int mode;
  Access client_access;
  int fd;
  int buffer_len;
  int filename_len;
  char* buffer;
  char* filename;
  int bytes_written;
  int return_code;

} Message;


int writeMessage(int fd, Message m);
int readMessage(int fd, Message* m);
