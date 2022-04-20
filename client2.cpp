#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#define MAX 1024
#define PORT 9034
#define SA struct sockaddr
void func(int sockfd)
{
    char buff[MAX] = "PUSH 0 client1";
    char check[MAX] = "0 client1";

    for (size_t i = 0; i < 1000000; i++)
    {
        write(sockfd, buff, sizeof(buff));
        buff[5] = i;
    }
    char sizebuf[MAX] = "size";
    write(sockfd, sizebuf, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("size of stack is: %s", buff);
    assert(strncmp(buff, "1000000", 7));

    bzero(buff, sizeof(buff));

    buff[0] = 'P';
    buff[1] = 'O';
    buff[2] = 'P';
    for (size_t i = 0; i < 1000000; i++)
    {
        write(sockfd, buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        assert(buff[0] == check[0]);
        check[0] = i;
    }

    char checkTOP[MAX] = "0 client1";
    bzero(buff, sizeof(buff));
    buff[0] = 'T';
    buff[1] = 'O';
    buff[2] = 'P';
    write(sockfd, buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    assert(buff[0] == checkTOP[0]);

    // char buff[MAX];
    // int n;
    // for (;;)
    // {
    //     bzero(buff, sizeof(buff));
    //     // printf("Enter the string : ");
    //     // n = 0;
    //     // while ((buff[n++] = getchar()) != '\n')
    //     //     ;
    //     write(sockfd, buff, sizeof(buff));
    //     if ((strncmp(buff, "exit", 4)) == 0 || (strncmp(buff, "EXIT", 4)) == 0)
    //     {
    //         printf("Client Exit...\n");
    //         break;
    //     }
    // }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
}