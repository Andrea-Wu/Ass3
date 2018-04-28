#ifndef LIBNETFILES_H_
#define LIBNETFILES_H_
  
int netserverinit(char* hostname);
int netopen(const char* pathname, int flags);
ssize_t netread(int fildes, void* buf, size_t nbyte);
ssize_t netwrite(int fildes, const void* buf, size_t nbyte);
int netclose(int fd);
int openCon();
  
#endif

