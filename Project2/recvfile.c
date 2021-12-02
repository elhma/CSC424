#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 4096

void fatal(char *string)
{
  printf("%s\n", string); 
  exit(1);
}

int main(int argc, char *argv[])
{
  int b, s, w, bytes, port = 1;
  int counter = 0;
  char buf[BUF_SIZE]; 
  struct hostent *h;
  struct sockaddr_in servAddr;
  unsigned int len;
  
  if (argc != 3) fatal("Usage: recvfile <recv-port> <filename>");
  port = strtol(argv[1], NULL, 10);
  
  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s <0) fatal("socket");
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port= htons(port);

 len = sizeof(servAddr);
  w = sendto(s, argv[2], strlen(argv[2])+1, 0, (struct sockaddr *) &servAddr, len);
  if(w < 0) fatal("send failed");
  
  while (1) {
    bytes = recvfrom(s,buf, BUF_SIZE,0, (struct sockaddr *) &servAddr, &len);
    printf("[recv data] %d (%d) ACCEPTED \n", counter, bytes);
           
    if (bytes <= 0) break;
           
    write(1, buf, bytes);
    counter += bytes;
  }
  
  printf("[completed] \n");
  close(s);
  return 0;
}
