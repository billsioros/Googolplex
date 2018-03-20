/* C++ Search Engine implementation by Vasileios Sioros */

#ifndef __ENGINE__
#define __ENGINE__

#include "trie.h"
#include <iosfwd>

class Engine
{
    static const unsigned maxQueries;
    
    Trie trie;
    
    // File Info Implementation:
    struct Info
    {
        const unsigned lines;     // File lines
        unsigned * const columns; // Each documents' length
        unsigned * const words;   // Each documents' word count
        char ** const documents;  // The documents content without the extra whitespace

        Info(const unsigned);
        ~Info();
    } const * info;

    const unsigned maxResults;
    double avgdl;
    const double k, b;

    Engine(std::ifstream&, const Info *, const unsigned, const double, const double);

    // Search Utility Functions:
    double score(const unsigned, const unsigned, const char * [], const PList * []) const;
    bool parseInput(char *, unsigned&, const char * [], const PList * []) const;
    void printResult(const unsigned, const double, const unsigned, const unsigned, const char * []) const;

public:
    
    enum Code {
        OK,
        CANNOT_OPEN_FILE,
        INVALID_ID_READ,
        ID_OUT_OF_RANGE,
        WORD_NOT_FOUND,
        NO_VALID_INPUT,
        EMPTY_DOC
    };

    ~Engine();

    static const Engine * validate(const char *, const unsigned, const double k = 1.2, const double b = 0.75);

    // Search Engine Functionality:
    void search(char *) const;
    void docfreq() const;
    void trmfreq(const int, const char *) const;
};

#endif
