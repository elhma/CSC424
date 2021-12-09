#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024 
#define QUEUE_SIZE 10

typedef struct StopAndWaitFrame {
  int seq;
  int bytes;
  int counter;
  char data[BUF_SIZE];
}sawFrame;

void fatal(char *string)
{
  printf("%s\n", string); 
  exit(1);
}

int main(int argc, char *argv[])
{
  int s, b, c, l, fd, bytes, on, r, port = 1;
  char buf[BUF_SIZE];
  int counter = 0;
  struct hostent *h;
  struct sockaddr_in servAddr;
  unsigned int len;
  
  if (argc != 4) fatal("Usage: sendfile <recv-host> <recv-port> <filename>");
  h = gethostbyname(argv[1]);
  port = strtol(argv[2], NULL, 10);

  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s<0) fatal("socket failed");
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  memcpy(&servAddr.sin_addr.s_addr, h->h_addr, h->h_length);
  servAddr.sin_port = htons(port);
  
  len = sizeof(servAddr);
  
  fd = open(argv[3], O_RDONLY);
  if (fd < 0) fatal ("open failed");
 
 fd_set readfds;
 struct timeval timeout;
 sawFrame send;
 send.seq = 0; 
 int ack = 0;
 int recvack;
 int success;

 while (1) {
 
   bytes= read(fd, buf, BUF_SIZE);
   strcpy(send.data, buf);
   send.bytes = bytes;
   send.seq += 1;
   send.counter = counter;
   
   sendto(s, &send, sizeof(sawFrame),0, (struct sockaddr *) &servAddr, len);
   printf("[send data] %d (%d) \n", counter, bytes);
   
   FD_ZERO( &readfds );   
   FD_SET ( s, &readfds );
   
   timeout.tv_sec = 5;     
   timeout.tv_usec = 0;
   success = 0;
   
   while(!success) {
     if ( select ( 32, &readfds, NULL, NULL, &timeout ) == 0 ) {
       sendto(s, &send, sizeof(sawFrame),0, (struct sockaddr *) &servAddr, len);
       printf("[resend data] %d (%d) \n", counter, bytes);
       
       FD_ZERO( &readfds );   
       FD_SET ( s, &readfds );
       
       timeout.tv_sec = 5;     
       timeout.tv_usec = 0;
       
     }
     else {
       recvfrom(s, &recvack, sizeof(recvack), 0, (struct sockaddr *) &servAddr, &len);
       printf("[recv ack] %d \n", recvack);
   
       FD_ZERO( &readfds );   
       FD_SET ( s, &readfds );
       success = 1;
     }
   }
   
   if (bytes <= 0) break;
   counter += bytes;
 }
  
  close(fd);
  printf("[completed] \n");
  
  return 0;
}
