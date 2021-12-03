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

#define BUF_SIZE 1024 
#define QUEUE_SIZE 10

typedef struct StopAndWaitFrame{
  int seq;
  char data[1024];
} sawFrame;

void fatal(char *string)
{
  printf("%s\n", string); 
  exit(1);
}

int main(int argc, char *argv[])
{
  int s, b, fd, bytes, r, port = 1;
  int counter = 0;
  char buf[BUF_SIZE];
  struct hostent *h;
  struct sockaddr_in servAddr;
  struct sockaddr_in cliAddr;
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
  
  b=bind(s, (struct sockaddr*) &servAddr, sizeof(servAddr)); 
  if (b< 0) fatal("bind failed");
  
  len = sizeof(cliAddr);
  r = recvfrom(s, buf, BUF_SIZE,0, (struct sockaddr *) &cliAddr, &len);
  if(r < 0) fatal("recv failed");
  
  fd = open(argv[3], O_RDONLY);
  if (fd < 0) fatal ("open failed");
    
  sawFrame send;
  send.seq = 0;
  int ack = 0;
  char recvstr[];
  int recvack;
  
  while (1) {
    if((ack == 0 && send.seq == 0) || (ack == 1 && send.seq == 1)){
      bytes= read(fd, buf, BUF_SIZE);
      strcpy(send.data, buf);
      send.seq = (send.seq+1)%2;
      
      sendto(s, &send, sizeof(sawFrame),0, (struct sockaddr *) &cliAddr, len);
      printf("[send data] %d (%d) \n", counter, bytes);
    }
    
    r = recvfrom(s,&recvstr,sizeof(int),0,(struct sockaddr *) &cliAddr, &len);
    recvack = strtol(recvstr);
    
    if(recvack == send.seq) {
      printf("[recvack] %d", recvack);
      ack = recvack;
    }
    else{
      printf("Didn't work");
    }
    
    if (bytes <= 0) break;
    counter += bytes;
  }
  
  close(fd);
  printf("[completed] \n");
  
  return 0;
}
