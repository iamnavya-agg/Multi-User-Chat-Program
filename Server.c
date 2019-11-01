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

struct client
{
	int index;
	int sockID;
	struct sockaddr_un clientAddr;
	int len;
};

struct client Client[1024];
int clientCount = 0;
pthread_t thread[1024];
int thread_error;

void * networking_function(void * ClientDetail)
{
	struct client* clientDetail = (struct client*) ClientDetail;
	int clientSocket = clientDetail -> sockID;
	int index = clientDetail -> index;

	printf("Client %d connected.\n",index + 1);

	while(TRUE)
	{

		char data[1024];
		char msg[1024];
		char output[1024];
		char error[1024];
		
		int read = recv(clientSocket,data,1024,0);
		if (read == -1)
		{
			perror("Data recieve failed.\nError ");
			return 0;
		}
		data[read] = '\0';
		
		if(strcmp("ALL",data) == 0)
		{
			read = recv(clientSocket,data,1024,0);
			if (read == -1)
			{
				perror("Data recieve failed.\nError ");
				return 0;
			}
			data[read] = '\0';
			sprintf(msg,"Client %d : %s",(index+1),data);
			strcpy(data,msg);
			for(int i=0; i < clientCount; i++)
			{
				if (send(Client[i].sockID,data,1024,0) < 0)
					{
						printf("Unable to send Message \n");
						continue;
					}
			}
		}
		if(strcmp("SEND",data) == 0)
		{

			read = recv(clientSocket,data,1024,0);
			if (read == -1)
			{
				perror("Data recieve failed.\nError ");
				return 0;
			}
			data[read] = '\0';

			int id = atoi(data) - 1;

			read = recv(clientSocket,data,1024,0);
			if (read == -1)
			{
				perror("Data recieve failed.\nError ");
				return 0;
			}
			data[read] = '\0';
			
			sprintf(msg,"Client %d : %s",(index+1),data);
			strcpy(data,msg);
			sprintf(error,"Server : Client %d not connected",(id+1));
			if (send(Client[id].sockID,data,1024,0) < 0)
			{
				printf("Unable to send Message \n");
				send(clientSocket,error,1024,0);
			}			

		}
		if(strcmp("CONNECTED",data) == 0)
		{

			int l = 0;

			for(int i = 0 ; i < clientCount ; i ++)
				l += snprintf(output + l,1024,"Client %d - socket %d.\n",i + 1,Client[i].sockID);

			if (send(clientSocket,output,1024,0) < 0)
				perror("Unable to send Message \n");
			continue;

		}
		if(strcmp("EXIT",data) == 0)
			printf("Client %d disconnected\n", (index+1));
	}
	return NULL;
}

int main()
{

	int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);								//Socket Creation (AF_UNIX)
	if (serverSocket == -1)
	{
		perror("Server Socket creation failed.\nError ");
		exit(1);
	}
	
	struct sockaddr_un serverAddr;													//Store addresses for UNIX Domain Socket(under <sys/un.h>)
	serverAddr.sun_family = AF_UNIX;												
	strcpy(serverAddr.sun_path, SocPath);
	unlink(SocPath);
	int len = strlen(serverAddr.sun_path) + sizeof(serverAddr.sun_family);

	if(bind(serverSocket,(struct sockaddr *) &serverAddr , len+1) == -1) 
	{
		perror("Bind failed.\nError ");
		return 0;
	}

	if(listen(serverSocket,1024) == -1)
	{
		perror("Listen failed.\nError ");
		return 0;
	}

	printf("Server started\nWaiting for clients to connect\n");

	while(TRUE)
	{

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;

		thread_error = pthread_create(&thread[clientCount], NULL, networking_function, (void *) &Client[clientCount]);
		if (thread_error != 0)
		{
			perror("Connection failed.\nError ");
			return 0;
		}

		clientCount ++;
 	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}
