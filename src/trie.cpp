/* C++ Trie (Inverted Index) implementation by Vasileios Sioros */

#include "trie.h"
#include "stack.h"
#include <cstring>
#include <iostream>
#include <cmath>

// Posting List Implementation:
PList::PList(const unsigned total)
:
IDF(0.0), documentNum(0), instances(new unsigned[total])
{
    for (unsigned i = 0; i < total; i++)
        instances[i] = 0;
}

PList::~PList()
{
    delete[] instances;
}

// Node Implementation:
Trie::Node::Node()
:
letter(nullptr), child(nullptr), sibling(nullptr), plist(nullptr)
{
}

Trie::Node::~Node()
{
    if (letter)
        delete letter;
        
    if (child)
        delete child;

    if (sibling)
        delete sibling;

    if (plist)
        delete plist;
}

// Using a stack in order to store "consumed" characters (prefix)
void Trie::Node::print() const
{
    static stack<char> pred;

    pred.push(*letter);

    if (plist)
        std::cout << pred << ' ' << plist->documentNum << std::endl;

    if (child)
        child->print();

    char rv; pred.pop(rv);
    
    if (sibling)
        sibling->print();
}

// Trie Implementation:
Trie::Trie(const unsigned total)
:
total(total)
{
}

// Used lexicographic sorting amongst siblings
void Trie::add(const char * string, const unsigned documentId)
{
    const unsigned len = std::strlen(string);

    Node * current = &root, * next = current->child;
    unsigned i = 0;

    // Traverse the trie (ex. "for" => 'f' -> 'o' -> 'r')
    // till the current letter isn' t a child or a child' s sibling
    while (next && i < len)
    {
        if (next->letter && *(next->letter) == string[i])
        {
            current = next; next = current->child; i++; continue;
        }
        
        for (next = next->sibling;
             next && next->letter && *(next->letter) < string[i];
             next = next->sibling);

        if (next && next->letter && *(next->letter) == string[i])
        {
            current = next; next = current->child; i++; continue;
        }

        next = nullptr;
    }

    // Create new nodes containing the remaining
    // letters of the string
    for (; i < len; i++)
    {
        Node * node = new Node();
        node->letter = new char(string[i]);

        if (!current->child)
        {
            current = current->child = node;
        }
        else
        {
            // Insert node in the list of siblings in a specific
            // position in order to maintain lexicographic sorting
            Node * tmp = nullptr, * sib = nullptr;
            for (sib = current->child;
                 sib->sibling && sib->sibling->letter && *(sib->sibling->letter) < *(node->letter);
                 sib = sib->sibling);

            tmp = sib->sibling;
            sib->sibling = node;
            node->sibling = tmp;

            current = node;
        }
    }

    // Update node' s Posting List & IDF
    if (!current->plist)
        current->plist = new PList(total);

    if (!current->plist->instances[documentId]++)
        current->plist->documentNum++;

    const double N = (double) total, n = (double) current->plist->documentNum;
    
    current->plist->IDF = std::log10((N - n + 0.5) / (n + 0.5));
}

// Used the same method of traversing
// the trie as before (add)
const PList * Trie::lookup(const char * string) const
{
    const unsigned len = std::strlen(string);
    
    const Node * current = &root, * next = current->child;
    unsigned i = 0;
    
    while (next && i < len)
    {
        if (next->letter && *(next->letter) == string[i])
        {
            current = next; next = current->child; i++; continue;
        }
        
        for (next = next->sibling;
             next && next->letter && *(next->letter) < string[i];
             next = next->sibling);

        if (next && next->letter && *(next->letter) == string[i])
        {
            current = next; next = current->child; i++; continue;
        }

        return nullptr;
    }

    // Return the current node' s Posting List
    // if every letter of the string has been consumed
    // i.e. we found the Posting List of the specified word
    return (i == len ? current->plist : nullptr);
}

void Trie::print() const
{
    const Node * next = root.child;

    if (next)
        next->print();
    
    for (next = root.sibling; next; next = next->sibling)
        next->print();
}
