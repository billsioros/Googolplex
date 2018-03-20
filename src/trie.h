/* C++ Trie (Inverted Index) implementation by Vasileios Sioros */

#ifndef __TRIE__
#define __TRIE__

// Posting List Implementation:
class PList
{
    friend class Trie;
    
    PList(const unsigned);
    ~PList();

public:
    
    double IDF;                 // Inverse Document Frequency
    unsigned documentNum;       // Counter of non-zero entries
    unsigned * const instances; // Pairs of: (1) Document IDs        (i)
                                // and       (2) Word Usage Counters (instances[i])
};

class Trie
{
    // Node Implementation:
    struct Node
    {
        char  * letter;         // A character within a certain word
        Node  * child;          // Next letter within the same word as the current
        Node  * sibling;        // Alternative contiunations of the same prefix
        PList * plist;          // Possibly unexistent posting list

        Node();
        ~Node();

        void print() const;
    } root;

    const unsigned total;       // Allocation size of this trie's PList::instances

public:
    
    Trie(const unsigned);

    void add(const char *, const unsigned);
    const PList * lookup(const char *) const;
    void print() const;
};

#endif
