#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024

typedef struct StopAndWaitFrame{
  int seq;
  char data[1024];
}sawFrame;

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
  struct sockaddr_in cliAddr;
  unsigned int len;
  
  if (argc != 2) fatal("Usage: recvfile <recv-port>");
  port = strtol(argv[1], NULL, 10);
  
  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s <0) fatal("socket");
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port= htons(port);

  b=bind(s, (struct sockaddr*) &servAddr, sizeof(servAddr)); 
  if (b< 0) fatal("bind failed");
  
  len = sizeof(cliAddr);
  
  sawFrame recv;
  int ack;
  
  while (1) {
    bytes = recvfrom(s,&recv, sizeof(sawFrame),0, (struct sockaddr *) &cliAddr, &len);
    printf("[recv data] %d (%ld) ACCEPTED \n", counter, sizeof(sawFrame));
    ack = htonl(recv.seq);
           
    sendto(s, &ack, sizeof(ack), 0, (struct sockaddr *) &cliAddr, len);
    if (bytes <= 0) break;
           
  //  write(1, recv.data, bytes);
    counter += sizeof(sawFrame);
  }
  
  printf("[completed] \n");
  close(s);
  return 0;
}
