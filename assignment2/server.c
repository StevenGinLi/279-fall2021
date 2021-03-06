// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    struct passwd* pwd;
    pid_t pid;
    
    //Check if Child
    if(strcmp(argv[0], "child") == 0)
    {
        new_socket = atoi(argv[1]);
        valread = read( new_socket , buffer, 1024);
        printf("%s\n", buffer);
        send(new_socket, hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
        return 0;
    }
    

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
	&opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_fd, (struct sockaddr *)&address,
	sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

	// Fork after socket connection is accepted
	pid = fork();
	
	if (pid == 0) {
		printf("Fork Success\n");
		
		// Set uid to nobody
        pwd = getpwnam("nobody");
        setuid(pwd->pw_uid);
		
		//Setup for info transfer to child
	    char child_socket[10];
        sprintf(child_socket, "%d", new_socket);
        char *new_args[] = {"child", child_socket, NULL};
        execvp(argv[0], new_args);
    } 
	
    return 0;
}
