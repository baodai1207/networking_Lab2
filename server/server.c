//
//  main.c
//  server
//
//  Created by Dai Huynh on 3/20/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <dirent.h>

#define MYPORT 5050 // the port users will be connecting to
#define BACKLOG 10  // how many pending connections queue will hold

void Run(char *option, int new_fd, char *buf, unsigned int len);

void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void)
{
    int sockfd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;

    char buf[100], option[8];
    int numbytes;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { // get the socket file
        perror("socket");
        exit(1);
    }
    printf("server: creating socket file \n");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(MYPORT);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP

    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1)
    {
        perror("bind");
        exit(1);
    }
    printf("server: binding the socket file to the local machine \n");
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("server: listening for incomming connections \n");
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

	printf("---------------------------------------------------\n");
    printf("Server has been started successfully\n");
	printf("---------------------------------------------------\n");

    while (1)
    {
        sin_size = sizeof their_addr;
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));

        if (!fork())
        {                  // this is the child process
            close(sockfd); // child doesn't need the listener
            while (1)
            {
                numbytes = recv(new_fd, buf, 100, 0);
                sscanf(buf, "%s", option);

                Run(option, new_fd, buf, numbytes);
            }

            close(new_fd); // close the connection
            exit(0);
        }
        close(new_fd); // parent doesn't need this
    }
    return 0;
}

void Run(char *option, int new_fd, char *buf, unsigned int len)
{
    struct stat obj;
    int filehandle, size, i, numbytes;
    char filename_src[20], filename_des[20];
    char buffer[1024];

    if (!strncmp(option, "catalog", 7))
    {
        printf("Received the option: CATALOG\n");

        chmod("tmp.txt", 0666);
        system("ls > tmp.txt");

        stat("tmp.txt", &obj);
        size = obj.st_size;

        filehandle = open("tmp.txt", O_RDONLY);
        // send the result from the file to the client
        sendfile(new_fd, filehandle, NULL, size);
    }

    else if (!strncmp(option, "spwd", 4))
    {
        printf("Received the option: SPWD\n");

        chmod("tmp.txt", 0666);
        system("pwd > tmp.txt");

        FILE *f = fopen("tmp.txt", "r");

        //Clear buffer
        for (i = 0; i < 100; i++)
            buf[i] = '\0';
        i = 0;

        //Copy FILE*f to buffer
        while (!feof(f))
            buf[i++] = fgetc(f);
        buf[i - 1] = '\0';

        fclose(f);

        //Send item in buffer
        send(new_fd, buf, 100, 0);
    }

    else if (!strncmp(option, "download", 8))
    {
        printf("Received the option: DOWNLOAD\n");
        bzero(buf,256);
        recv(new_fd, buf, 800, 0);
        // sscanf(buf, "%s", filename_src);

        printf("Prepare to upload: %s\n", filename_src);
        FILE *fd = fopen(filename_src, "rb");

        // if ((fd = fopen(filename_des, "rb")) != NULL)
        // {
        //     stat(filename_des, &obj);
        //     filehandle = open(filename_des, O_RDONLY);
        //     size = obj.st_size;
        //     sendfile(new_fd, filehandle, NULL, size);
        //     printf("Sent the file!\n");
        //     fclose(fd);
        // }
        // else
        // {
        //     for (i = 0; i < 100; i++)
        //         buf[i] = '\0';
        //     i = 0;

        //     send(new_fd, buf, 100, 0);
        //     perror("Download Failed");
        // }
        int fileSize;

        fseek(fd, 0L, SEEK_END); //find
        fileSize = ftell(fd); //the
        rewind(fd); //filesize

        char fileBuffer[fileSize]; //initalize buffer to appropriate size

        // char sizeAccept = to_string(fileSize); //prepare size for sending

        // cout << fileSize << endl;

        if((numbytes = write(new_fd, fileBuffer, fileSize) < 0)) //Send size
			printf("Error");

        fread( fileBuffer , fileSize, 1 , fd); //read in  the file

       if((numbytes = write(new_fd, fileBuffer, fileSize)) < 0) //Send the file
			printf("Error");


        fclose(fd); //close the file
    }

    /*
	else if(!strncmp(option, "upload", 6))
	{
		printf("Received the option: UPLOAD\n");
		numbytes = recv(new_fd, buffer, 800, 0);
		
		if(numbytes == -1)
		{
			perror("FILE NOT FOUND.");
			return 0;
		}
		else
		{
			while(1)
			{
				flag1 = open(FilePath, O_CREAT | O_EXCL | O_WRONLY, 0666);
				if(flag1 == -1)	
					sprintf(FilePath + strlen(FilePath), "%d", n);
				else			
					break;
			}
			write(flag1, t, numbytes, 0);
			close(flag1);

			strcpy(bufferTMP, "cat ");
			strcat(bufferTMP, FilePath);
			printf("\n\n\nThe file has been downloaded\n");
			system(bufferTMP);
		}
		
	}
	*/
    else if (!strncmp(option, "bye", 3))
    {
        printf("Received the option: BYE\n");
        printf("Client Disconnected \n");
        exit(0);
    }
}