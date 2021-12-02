#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 12355
#define BUF_SIZE 4096

void fatal(char *string)
{
  printf("%s\n", string); 
  exit(1);
}

int main(int argc, char *argv[])
{
  int b, s, w, bytes = 1;
  char buf[BUF_SIZE]; 
  struct hostent *h;
  struct sockaddr_in servAddr;
  struct sockaddr_in cliAddr;
  unsigned int len;
  
  if (argc != 3) fatal("Usage: client server-name file-name");
  h = gethostbyname(argv[1]); 
  
  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s <0) fatal("socket");
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  memcpy(&servAddr.sin_addr.s_addr, h->h_addr, h->h_length);
  servAddr.sin_port= htons(SERVER_PORT);
  
  b=bind(s, (struct sockaddr*) &servAddr, sizeof(servAddr)); 
  if (b< 0) fatal("bind failed");

 len = sizeof(cliAddr);
  w = sendto(s, argv[2], strlen(argv[2])+1, 0, (struct sockaddr *) &cliAddr, len);
  if(w < 0) fatal("send failed");
  
  while (1) {
    bytes = recvfrom(s,buf, BUF_SIZE,0, (struct sockaddr *) &cliAddr, &len);
//    printf("receiving: %s", buf);
//    printf("recv %d\n", bytes);
    if (bytes <= 0) exit(0);
    write(1, buf, bytes);
  }
  
  close(s);
  return 0;
}
