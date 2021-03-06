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
  
  FILE *fp;
  fp = fopen("output.txt", "w+");
  
  sawFrame recv;
  int seq;
  int sendack;
  int ack = 0;
  int accepted;
  
  while (1) {
    bytes = recvfrom(s,&recv, sizeof(sawFrame),0, (struct sockaddr *) &cliAddr, &len);   
    
    if(recv.seq == ack+1) {
      printf("[recv data] %d (%d) ACCEPTED \n", recv.counter, recv.bytes); 
      fwrite(recv.data, recv.bytes, 1, fp);
      ack = recv.seq;
      accepted = 1;
    }
    else {
      printf("[recv data] %d (%d) IGNORED \n", recv.counter, recv.bytes);
      accepted = 0;
    }
    
    sendack = recv.seq;
    sendto(s, &sendack, sizeof(sendack), 0, (struct sockaddr *) &cliAddr, len);
    
    if ((recv.bytes == 0) && (accepted == 1)) break;
  }
  
  printf("[completed] \n");
  fclose(fp);
  close(s);

}
