/* C++ Stack implementation by Vasileios Sioros */

#ifndef __STACK__
#define __STACK__

#include <fstream>

template <typename T>
class stack
{
    struct node
    {
        const T data;
        node * next;

        node(const T&);
        ~node();
    } * head, * tail;

public:

    stack();
    ~stack();

    void push(const T&);
    bool pop(T&);

    friend std::ostream& operator<<(std::ostream& out, const stack& s)
    {
        for (const node * current = s.head; current; current = current->next)
            out << current->data;

        return out;
    }
};

// Node Implementation:
template <typename T>
stack<T>::node::node(const T& data)
:
data(data), next(nullptr)
{
}

template <typename T>
stack<T>::node::~node()
{
    if (next)
        delete next;
}

// Stack Implementation:
template <typename T>
stack<T>::stack()
:
head(nullptr), tail(nullptr)
{
}

template <typename T>
stack<T>::~stack()
{
    if (head)
        delete head;
}

template <typename T>
void stack<T>::push(const T& data)
{
    if (!head)
        head = tail = new node(data);
    else
        tail = tail->next = new node(data);
}

template <typename T>
bool stack<T>::pop(T& data)
{
    if (head)
    {
        if (head->next)
        {
            node * ntail = head;
            for (; ntail->next != tail; ntail = ntail->next);

            data = tail->data;
            delete tail;
            tail = ntail; ntail->next = nullptr;
        }
        else
        {
            data = head->data;
            delete head;
            head = tail = nullptr;
        }

        return true;
    }

    return false;
}

#endif
