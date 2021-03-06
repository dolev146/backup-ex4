#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include "Stack.hpp"

using namespace ex4;

int main()
{
    // Stack *mystack = new Stack();
    // std::string a = "yakov is great";
    // std::string b = "dolev is great";
    // mystack->push(a);
    // mystack->push(b);
    // std::string out = mystack->top();
    // std::cout << out << std::endl;
    // out = mystack->pop();
    // std::cout << out << std::endl;
    // out = mystack->pop();
    // std::cout << out << std::endl;

    int server_pid = fork();
    int client_1 = fork();
    int client_2 = fork();

    if (client_1 == -1)
    {
        return 1;
    }
    if (server_pid == -1)
    {
        return 1;
    }
    if (client_2 == -1)
    {
        return 1;
    }
    std::string param = "./server";
    std::string client1_param = "./client1";
    std::string client2_param = "./client2";
    if (server_pid == 0)
    {
        // child process
        char *args_server[] = {(char *)(param.c_str()), NULL};
        execv(args_server[0], args_server);
    }
    else
    {
        // parent process
        // wait(NULL);
    }

    if (client_1 == 0)
    {
        // child process
        char *args_client1[] = {(char *)(client1_param.c_str()), NULL};
        execv(args_client1[0], args_client1);
    }
    if (client_2 == 0)
    {
        // child process
        char *args_client2[] = {(char *)(client2_param.c_str()), NULL};
        execv(args_client2[0], args_client2);
    }

    wait(NULL);
    return 0;
}