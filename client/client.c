//
//  main.c
//  client
//
//  Created by Dai Huynh on 04/12/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stdbool.h>

#define PORT 5050		// the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

int sockfd, numbytes;
bool ServerCommands(char *option);
void ClientCommands(char *option);
bool client_commands();
void Menu();

int main(int argc, char *argv[])
{
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information
	if (argc != 2)
	{
		fprintf(stderr, "usage: client hostname\n");
		exit(1);
	}
	if ((he = gethostbyname(argv[1])) == NULL)
	{ // get the host info
		herror("gethostbyname");
		exit(1);
	}
	printf("client: get the server info \n");
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{ // get the socket file
		perror("socket");
		exit(1);
	}
	printf("client: creating socket file \n");
	their_addr.sin_family = AF_INET;					   // host byte order
	their_addr.sin_port = htons(PORT);					   // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr); // store server ip addess (from the
														   // gethostbyname function)
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

	if (connect(sockfd, (struct sockaddr *)&their_addr, // connect to server
				sizeof their_addr) == -1)
	{
		perror("connect");
		exit(1);
	}
	printf("client: connecting to the server \n");

	printf("---------------------------------------------------\n");
	Menu();
	printf("---------------------------------------------------\n");

	while (1)
	{
		if (!client_commands())
			break;
	}
}

bool client_commands()
{
	char option[500];

	scanf("%s", option);
	fflush(stdin);

	printf("---------------------------------------------------\n");

	if (!strncmp(option, "help", 4))
	{
		Menu();
	}

	else if (!strncmp(option, "catalog", 2) ||
			 !strncmp(option, "download", 8) ||
			 !strncmp(option, "upload", 6) ||
			 !strncmp(option, "spwd", 4) ||
			 !strncmp(option, "bye", 3))
	{
		if (!ServerCommands(option))
			return 0;
	}

	else if (!strncmp(option, "ls", 2) ||
			 !strncmp(option, "pwd", 3))
	{
		ClientCommands(option);
	}

	else
	{
		printf("'%s' option is not valid.\n", option);
	}

	printf("---------------------------------------------------\n");

	return 1;
}

bool ServerCommands(char *option)
{
	char buffer[500], fileName[50], t[1000];
	int numbytes, num, index, n = 1;

	if (!strncmp(option, "catalog", 7))
	{
		strcpy(buffer, "catalog");
		send(sockfd, buffer, strlen(buffer) + 1, 0);

		numbytes = recv(sockfd, t, 1000, 0);

		printf("%s", t);
	}

	// else if (strncmp(buffer,"download",8) == 0){}

	else if (!strncmp(option, "download", 8))
	{
		char filename_src[20], filename_des[20];
		bzero(buffer, 256);
		printf("\nType the full path of file to download: ");
		scanf("%s", filename_des);

		strcpy(buffer, "download ");
		strcat(buffer, filename_des);

		send(sockfd, buffer, strlen(buffer) + 1, 0);

		FILE *fd = fopen(filename_des, "wb");

		if ((numbytes = read(sockfd, buffer, 255)) < 0) //read size
			printf("Error");

		int fileSize = atoi(buffer);

		char fileBuffer[fileSize];

		if ((numbytes = read(sockfd, fileBuffer, fileSize + 1)) < 0) //read file
			printf("Error");

		fwrite(fileBuffer, 1, fileSize, fd); //save file

		printf("\n\n\nThe file has been downloaded\n");
		// system(buffer);
		fclose(fd);
	}

	// else if (!strncmp(option, "upload", 6))
	// {
	// 	printf("\n Type the path of file to upload: ");
	// 	scanf("%s", fileName);
	// 	strcpy(buffer, "upload ");
	// 	strcat(buffer, fileName);
	// 	send(sockfd, buffer, strlen(buffer) + 1, 0);
	// 	numbytes = recv(sockfd, t, 800, 0);
	// 	if (t[0] == '\0')
	// 	{
	// 		perror("File is not existing.");
	// 		return 0;
	// 	}
	// 	else
	// 	{
	// 		while (1)
	// 		{
	// 			index = open(fileName, O_CREAT | O_EXCL | O_WRONLY, 0666);
	// 			if (index == -1)
	// 				sprintf(fileName + strlen(fileName), "%d", n);
	// 			else
	// 				break;
	// 		}
	// 		write(index, t, numbytes);
	// 		close(index);

	// 		strcpy(buffer, "cat ");
	// 		strcat(buffer, fileName);
	// 		printf("\n\n\nThe file has been uploaded\n");
	// 		system(buffer);
	// 	}
	// }

	else if (!strncmp(option, "spwd", 4))
	{
		strcpy(buffer, "spwd");
		send(sockfd, buffer, strlen(buffer) + 1, 0);
		numbytes = recv(sockfd, buffer, 500, 0);
		printf("%s", buffer);
	}

	else if (!strncmp(option, "bye", 3))
	{
		strcpy(buffer, "bye");
		send(sockfd, buffer, strlen(buffer) + 1, 0);
		numbytes = recv(sockfd, &num, 500, 0);
		if (numbytes != -1)
		{
			printf("\n The client has been disconnected.\n");
			exit(0);
		}
		else
			printf("\n Server does not respond.\n");
	}

	return 1;
}

void ClientCommands(char *option)
{
	char buffer[500];

	if (!strncmp(option, "ls", 3))
	{
		system("ls");
	}

	else if (!strncmp(option, "pwd", 4))
	{
		system("pwd");
	}

	else if (!strncmp(option, "help", 4))
	{
		Menu();
	}
}

void Menu()
{
	printf("List of commands: \n");
	printf("- Type 'catalog' to show files at the server's current directory \n");
	printf("- Type 'ls' to show files at the client's current directory \n");
	printf("- Type 'download' to download file from server \n");
	printf("- Type 'upload' to upload file to server \n");
	printf("- Type 'spwd' to display server's current directory \n");
	printf("- Type 'pwd' to display client's current directory \n");
	printf("- Type 'help' to show all commands \n");
	printf("- Type 'bye' to close the connection from the server \n");
}
