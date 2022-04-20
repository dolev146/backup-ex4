#include "Stack.hpp"
#define MAX_LIMIT 1024
#include <string.h>

namespace ex4
{
    char str[MAX_LIMIT];

    int Stack::getsize()
    {
        return this->size;
    }

    std::string Stack::pop()
    {
        std::string txt = this->head->txt;
        Node *temp = this->head;
        this->head = this->head->next;
        this->size--;
        delete temp;
        return txt;
    }
    std::string Stack::top()
    {
        return this->head->txt;
    }

    void Stack::push(std::string &txt)
    {
        memcpy(str, txt.c_str() + 5, MAX_LIMIT - 5);
        std::string cppstr = std::string(str);
        std::cout
            << "in push: " << cppstr << std::endl;
        // std::cout <<  << std::endl;
        Node *n = new Node(cppstr);
        if (this->head == NULL)
        {
            this->head = n;
        }
        else
        {
            n->next = this->head;
            this->head = n;
        }
        this->size++;

        return;
    };

}