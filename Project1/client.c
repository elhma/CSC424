#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "common.h"
#define pAssert(a,b) {if (!(a)) {printf("Line %d in File %s: %s\n", __LINE__, __FILE__, b); exit(1);}}
#define pError(e,a) {if (e) {perror(a); exit(1);}}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    char client_msg[MAX_STR_SIZE];
    char server_msg1[MAX_STR_SIZE];
    char server_msg2[MAX_STR_SIZE];
    char magic_str[] = MAGIC_STRING;
    char *addr_string = 0x0, *rand_num_str = 0x0, *rand_num_str2 = 0x0, *gold_str = 0x0;



    if (argc < 2) {
       fprintf(stderr,"usage %s cid\n", argv[0]);
       exit(0);
    }

    /* Establish Connection*/
    portno = SERVER_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    printf("checkpoint1");
	
	//Complete the missing codes
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = atoi(SERVER_ADDR);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    printf("checkpoint2");
	
    /* Communication */
    /* construct HELLO message */
    bzero(client_msg, MAX_STR_SIZE);
    sprintf(client_msg, "%s HELLO %s\n", magic_str, argv[argc-1]);	
    printf("%s HELLO %s\n", magic_str, argv[argc-1]);

    /* send HELLO message */
    n = write(sockfd, client_msg, MAX_STR_SIZE);
    if (n < 0)
	    error("ERROR writing to socket");

    /* receive STATUS message */
    bzero(server_msg1,MAX_STR_SIZE);					//receive STATUS message
    n = read(sockfd, server_msg1, MAX_STR_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    
    /* parse STATUS message */
    char* check_str1 = strtok(server_msg1, " ");
    if (strcmp(check_str1, magic_str) != 0)
	error("Incorrect magic string");

    char* check_typ1 = strtok(NULL, " ");
    if (strcmp(check_typ1, "STATUS") != 0)
	error("Incorrect message type");

    rand_num_str = strtok(NULL, " ");
    rand_num_str2 = strtok(NULL, " ");
    addr_string = strtok(NULL, " ");
    printf("%s STATUS %s %s %s\n", magic_str, rand_num_str, rand_num_str2, addr_string);


    /* construct BYE message */
    bzero(client_msg, MAX_STR_SIZE);
    sprintf(client_msg, "%s BYE %d\n", magic_str, atoi(rand_num_str) + atoi(rand_num_str2)); 
    printf("%s BYE %d\n", magic_str, atoi(rand_num_str) + atoi(rand_num_str2));


    /* send BYE message */
    n = write(sockfd, client_msg, MAX_STR_SIZE);
    if (n < 0)
	    error("ERROR writing to socket");


    /* receive CONFIRM_BYE message */
    bzero(server_msg2,MAX_STR_SIZE);					
    n = read(sockfd, server_msg2, MAX_STR_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    
    /* parse CONFIRM_BYE message */
    char* check_str2 = strtok(server_msg1, " ");
    if (strcmp(check_str2, magic_str) != 0)
	error("Incorrect magic string");

    char* check_typ2 = strtok(NULL, " ");
    if (strcmp(check_typ2, "CONFIRM_BYE") != 0)
	error("Incorrect message type");

    gold_str = strtok(NULL, " ");
  
    printf("%s CONFIRM_BYE %s \n", magic_str, gold_str);

    /* Close Connection */
    close(sockfd);
    return 0;
}
