#ifndef HEADER_H
#define HEADER_H

// #include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <thread>

namespace ex4
{

    class Node
    {
    public:
        std::string txt;
        Node *next;

        Node(std::string &txt)
        {

            this->txt = txt;
            this->next = NULL;
        };
    };

    class Stack
    {
    public:
        Node *head = NULL;
        int size = 0;
        std::string pop();
        std::string top();
        void push(std::string &txt);
        int getsize();
    };
}
#endif // HEADER_H