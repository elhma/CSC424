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
  
  if (argc != 4) fatal("Usage: sendfile <recv-host> <recv-port> <filename>");
  h = gethostbyname(argv[1]);
  port = strtol(argv[2], NULL, 10);
  
  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s <0) fatal("socket");
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  memcpy(&servAddr.sin_addr.s_addr, h->h_addr, h->h_length);
  servAddr.sin_port= htons(port);

 len = sizeof(servAddr);
  w = sendto(s, argv[3], strlen(argv[3])+1, 0, (struct sockaddr *) &servAddr, len);
  if(w < 0) fatal("send failed");
  
  while (1) {
    bytes = recvfrom(s,buf, BUF_SIZE,0, (struct sockaddr *) &servAddr, &len);
    printf("[recv data] %d (%d) ACCEPTED \n", counter, bytes);
           
    if (bytes <= 0) exit(0);
           
    write(1, buf, bytes);
    counter += bytes;
  }
  
  printf("[completed]");
  close(s);
  return 0;
}
