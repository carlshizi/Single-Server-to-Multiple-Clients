/*****Server*****/

/*
 *                                                              
 *	  ********SINGLE SERVER CONNECTION TO MULTIPLE CLIENTS********
 *	  Program sets up a single server connection to one or more
 *	  clients. If the server receives "shutdown" from a client,
 *	  the server shuts down the socket corresponding to the client
 *	  and the client exits.
 *	  ************************************************************
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define LISTEN_PORT 8080

int main(){

	int sock_listen, binder, new_connect, result;
	struct sockaddr_in my_addr, recv_addr;
	socklen_t addr_size;
	char buf[BUF_SIZE];
	pid_t child;

	// socket()
	sock_listen = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_listen < 0){
		printf("socket() failed\n");
		exit(1);
	}

	memset(&my_addr, '\0', sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(LISTEN_PORT);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// bind()
	binder = bind(sock_listen, (struct sockaddr*)&my_addr, sizeof(my_addr));
	if(binder < 0){
		printf("bind() failed\n");
		exit(1);
	}
	puts("Waiting for client...");

	// listen()
	if(listen(sock_listen, 10) < 0){
		printf("listen() failed\n");
	}

	// accept()
	while(1){
		new_connect = accept(sock_listen, (struct sockaddr*) &recv_addr, &addr_size);
		if(new_connect < 0){
			exit(1);
		}

		printf("[%s:%d Connected]\n", inet_ntoa(recv_addr.sin_addr), ntohs(recv_addr.sin_port));
		
		if((child = fork()) == 0){
			close(sock_listen);

			while(1){
				result = recv(new_connect, buf, BUF_SIZE, 0);

				// Error handling to prevent infinite loop when child Ctrl-C
				if(result == -1){
					break;
				}

				if(result == 0){
					printf("[Client %s:%d Interrupted]\n", inet_ntoa(recv_addr.sin_addr),
							ntohs(recv_addr.sin_port));
					break;
				}
				buf[strcspn(buf, "\n")] = '\0';
				
				// Receives shutdown signal from client
				if(strcmp(buf, "shutdown") == 0){
					printf("[Client %s:%d Disconnected]\n", inet_ntoa(recv_addr.sin_addr),
							ntohs(recv_addr.sin_port));
					break;
				}
				else{
					printf("Received: %s\n", buf);
					send(new_connect, buf, strlen(buf), 0);
					bzero(buf, sizeof(buf));
				}
			}
		}
	}
	close(new_connect);
	return 0;
}
