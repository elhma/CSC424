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

#define SERVER_PORT 12345
#define BUF_SIZE 4096 
#define QUEUE_SIZE 10

void fatal(char *string)
{
  printf("%s\n", string); 
  exit(1);
}

int main(int argc, char *argv[])
{
  int s, b, c, l, fd, bytes, on, r = 1;
  char buf[BUF_SIZE];
  struct sockaddr_in channel;

  memset(&channel, 0, sizeof(channel));
  channel.sin_family= AF_INET;
  channel.sin_addr.s_addr =htonl(INADDR_ANY);
  channel.sin_port = htons(SERVER_PORT);

  s=socket(AF_INET, SOCK_DGRAM, 0);
  if (s<0) fatal("socket failed");
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)); 
  
  b=bind(s, (struct sockaddr*) &channel, sizeof(channel)); 
  if (b< 0) fatal("bind failed");

  c=connect(s, (struct sockaddr*) &channel, sizeof(channel)); 
  if (c< 0) fatal("connect failed");
  
  r = recv(s, buf, BUF_SIZE,0);
  if(r < 0) fatal("recv failed");
    
  fd = open(buf, O_RDONLY);
  if (fd < 0) fatal ("open failed");
    
  while (1) {
    bytes= read(fd, buf, BUF_SIZE);
    printf("send %d\n", bytes);
    if (bytes <= 0) break;
    send(s, buf, bytes,0);
    }
  close(fd);
  close(s);
  
  exit(0);
  
}
