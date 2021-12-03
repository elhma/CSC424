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
  int s, w, bytes, port = 1;
  int counter = 0;
  char buf[BUF_SIZE]; 
  struct hostent *h;
  struct sockaddr_in servAddr;
  unsigned int len;
  
  if (argc != 2) fatal("Usage: recvfile <recv-port>");
  port = strtol(argv[1], NULL, 10);
  
  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s <0) fatal("socket");
  
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family= AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port= htons(port);

  len = sizeof(servAddr);
  w = sendto(s, argv[1], strlen(argv[1])+1, 0, (struct sockaddr *) &servAddr, len);
  if(w < 0) fatal("send failed");
  
  sawFrame recv;
  long ack;
  while (1) {
    bytes = recvfrom(s,&recv, sizeof(sawFrame),0, (struct sockaddr *) &servAddr, &len);
    printf("[recv data] %d (%d) ACCEPTED \n", counter, bytes);
           
    ack = recv.seq;
    sendto(s, &ack, sizeof(int), 0, (struct sockaddr *) &servAddr, len);
    if (bytes <= 0) break;
           
    write(1, recv.data, sizeof(sawFrame));
    counter += bytes;
  }
  
  printf("[completed] \n");
  close(s);
  return 0;
}
