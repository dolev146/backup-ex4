/*
** server.c -- a stream socket server demo
*
*   Thread version
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "Stack.hpp"
#include <mutex>
#define PORT "9034" // the port users will be connecting to
#define BACKLOG 10  // how many pending connections queue will hold
using namespace ex4;

class param_args
{
public:
    int fd;
    Stack *param_stack;
};

char buf[1024]; // Buffer for client data

std::mutex m;

void *thread_controller(void *var)
{

    int new_fd;
    param_args *ptr = (param_args *)var;
    new_fd = ptr->fd;
    Stack my_stack = *ptr->param_stack;
    for (;;)
    {

        if (recv(new_fd, buf, sizeof buf, 0) == -1)
        {
            perror("recv");
        }

        if (strlen(buf) != 0)
        {
            m.lock();
            if (strncmp(buf, "PUSH", 4) == 0)
            {
                printf("from client : %s", buf);
                std::string str_buff = std::string(buf);
                my_stack.push(str_buff);
            }
            else if (strncmp(buf, "POP", 3) == 0)
            {
                printf("from client : %s \n", buf);
                std::string output = my_stack.pop();
                strncpy(buf, output.c_str(), sizeof(output));
                send(new_fd, buf, sizeof(buf), 0);
            }
            else if (strncmp(buf, "TOP", 3) == 0)
            {
                printf("from client : %s \n", buf);
                std::string output = my_stack.top();
                strncpy(buf, output.c_str(), sizeof(output));
                send(new_fd, buf, sizeof(buf), 0);
            }
            else if (strncmp(buf, "size", 4) == 0)
            {
                printf("from client : %s \n", buf);
                int output = my_stack.getsize();
                bzero(buf, sizeof(buf));
                sprintf(buf, "%d", output);
                send(new_fd, buf, sizeof(buf), 0);
            }
            else if (strncmp(buf, "exit", 4) == 0 || strncmp(buf, "EXIT", 4) == 0)
            {
                printf("closed fd : %d \n", new_fd);
                close(new_fd);
                m.unlock();
                return NULL;
            }
            m.unlock();
        }
    }
    // close(new_fd);
    return NULL;
}

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1)
    {
        // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept ");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        Stack *my_stack = new Stack();
        param_args para;
        para.fd = new_fd;
        para.param_stack = my_stack;

        pthread_t t1; // we create here a thread to send the message
        pthread_create(&t1, NULL, thread_controller, &para);
        // pthread_join(t1, NULL);
    }

    return 0;
}
