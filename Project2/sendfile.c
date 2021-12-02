#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUF_SIZE 4096 
#define QUEUE_SIZE 10

void fatal(char *string)
{
  printf("%s\n", string); 
  exit(1);
}

int main(int argc, char *argv[])
{
  int s, b, c, l, fd, bytes, on, r, port = 1;
  int counter = 0;
  char buf[BUF_SIZE];
  struct sockaddr_in servAddr;
  struct sockaddr_in cliAddr;
  unsigned int len;
  
  if (argc != 2) fatal("Usage: recvfile <recv-port>");
  port = strtol(argv[1], NULL, 10);
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  servAddr.sin_addr.s_addr =htonl(INADDR_ANY);
  servAddr.sin_port = htons(port);

  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s<0) fatal("socket failed");
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)); 
  
  b=bind(s, (struct sockaddr*) &servAddr, sizeof(servAddr)); 
  if (b< 0) fatal("bind failed");
  
  len = sizeof(cliAddr);
  r = recvfrom(s, buf, BUF_SIZE,0, (struct sockaddr *) &cliAddr, &len);
  if(r < 0) fatal("recv failed");

  fd = open(buf, O_RDONLY);
  if (fd < 0) fatal ("open failed");
    
 while (1) {
   bytes= read(fd, buf, BUF_SIZE);
   
   sendto(s, buf, bytes,0, (struct sockaddr *) &cliAddr, len);
   printf("[send data] %d (%d) \n", counter, bytes);
   
   if (bytes <= 0) break;
   counter += bytes;
 }
  
  close(fd);
  printf("[completed]");
  
  return 0;
}
