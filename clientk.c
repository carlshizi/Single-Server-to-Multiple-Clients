/*****CLIENT*****/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main()
{
	int sock_send, i;
	int send_len, bytes_sent;
	char text[BUF_SIZE];
	struct sockaddr_in addr_send;

	/* create socket for sending data */
	sock_send = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_send < 0)
	{
		printf("socket() failed\n");
		exit(0);
	}

	/* create socket address structure to connect to */
	memset(&addr_send, '\0', sizeof(addr_send));
	addr_send.sin_family = AF_INET;
	addr_send.sin_port = htons(SERVER_PORT);
	addr_send.sin_addr.s_addr = inet_addr(SERVER_IP);

	/* connect to server */
	i = connect(sock_send, (struct sockaddr *) &addr_send, sizeof(addr_send));	
	if (i < 0)
	{
		printf("connect() failed\n");
		exit(1);
	}
	printf("Connection established.\n");


	while(1){
		printf("Message: ");
		fgets(text, sizeof(text), stdin);	
		send(sock_send, text, strlen(text), 0);
		text[strcspn(text, "\n")] = '\0';

		if(strcmp(text, "shutdown") == 0){
			close(sock_send);
			printf("Connection terminated.\n");
			exit(1);
		}
		
		if(recv(sock_send, text, 1024, 0) < 0){
			printf("Message not sent\n");
			break;
		}
		else{
			printf("Message sent!\n");
		}
	}

	close(sock_send);
	return 0;
}
