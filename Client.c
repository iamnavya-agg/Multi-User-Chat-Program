#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define TRUE 1
#define SocPath "File"

void * Recieve_data(void * sockID)
{

	int clientSocket = *((int *) sockID);

	while(TRUE)
	{
		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		if (read == -1)
		{
			perror("Data recieve failed.\nError ");
			return 0;
		}
		data[read] = '\0';
		printf("%s\n",data);
	}

}

int main()
{

	int clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clientSocket == -1)
	{
		perror("Server Socket creation failed\n");
		exit(1);
	}

	struct sockaddr_un serverAddr;

	serverAddr.sun_family = AF_UNIX;
	strcpy(serverAddr.sun_path, SocPath);
	
	int con = connect(clientSocket,(struct sockaddr*) &serverAddr, sizeof(serverAddr));
	if(con == -1) 
	{
		perror("Connection failed.\nError ");
		return 0;
	}

	printf("Connection established ............\n");

	pthread_t thread;
	int thread_error = pthread_create(&thread, NULL, Recieve_data, (void *) &clientSocket );
	if (thread_error != 0)
	{
		perror("Connection failed.\nError ");
		return 0;
	}

	while(TRUE)
	{

		char input[1024];
		scanf("%s",input);

		if(strcmp(input,"ALL") == 0)
		{

			send(clientSocket,input,1024,0);
			
			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);

		}
		else if(strcmp(input,"SEND") == 0)
		{

			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);
			
			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);

		}
		else if(strcmp(input,"CONNECTED") == 0)
		{

			send(clientSocket,input,1024,0);

		}
		else if(strcmp(input,"EXIT") == 0)
		{
			send(clientSocket,input,1024,0);
			printf("Connection terminated ..........\n");
			break;
		}
		else
		{
			printf("Server : Invalid input\n");
		}
		
	}
}