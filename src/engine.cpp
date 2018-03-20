/* C++ Search Engine implementation by Vasileios Sioros */

#include "engine.h"
#include "trie.h"
#include "heap.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <cctype>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iomanip>

// Error Messages:
static const char * Message[] =
{
    [Engine::Code::OK]               = "<Message>: OK",
    [Engine::Code::CANNOT_OPEN_FILE] = "<Error>: Unable to open the specified file",
    [Engine::Code::INVALID_ID_READ]  = "<Error>: Document ID of negative value or in the wrong order",
    [Engine::Code::ID_OUT_OF_RANGE]  = "<Error>: Document ID out of range",
    [Engine::Code::WORD_NOT_FOUND]   = "<Error>: No occurrences of the specified word",
    [Engine::Code::NO_VALID_INPUT]   = "<Error>: No valid input",
    [Engine::Code::EMPTY_DOC]        = "<Error>: A document appears to be empty"
};

// File Info Implementation:
Engine::Info::Info(const unsigned lines)
:
lines(lines), columns(new unsigned[lines]), words(new unsigned[lines]), documents(new char*[lines])
{
    for (unsigned i = 0; i < lines; i++)
        words[i] = 0;
}

Engine::Info::~Info()
{
    for (unsigned id = 0; id < lines; id++)
        delete[] documents[id];
        
    delete[] documents;
    delete[] words;
    delete[] columns;
}

// Search Engine Implementation:
const unsigned Engine::maxQueries = 10U;

Engine::Engine(std::ifstream& ifs, const Info * info, const unsigned maxResults, const double k, const double b)
:
trie(info->lines), info(info), maxResults(maxResults), avgdl(0.0), k(k), b(b)
{
    double sum = 0.0;
    for (unsigned id = 0; id < info->lines; id++)
    {
        char word[512]; ifs >> word; // Consume Document ID

        const unsigned len = info->columns[id];
        unsigned end = 0;
        
        // Read each word of the document
        info->documents[id][end] = '\0';
        do
        {
            ifs >> word; trie.add(word, id); info->words[id]++;
            
            std::strcat(info->documents[id], word);
            end += std::strlen(word);
            if (end < len - 1)
            {
                info->documents[id][end] = ' ';
                info->documents[id][++end] = '\0';
            }
            else
            {
                info->documents[id][end] = '\0';
                break;
            }            
        } while (ifs.good());

        sum += (double) info->words[id];
    }

    avgdl = sum / (double) info->lines;
}

Engine::~Engine()
{
    delete info;
}

// Given a filename validate that the specified file
// fulfill the requirements i.e. non negative document IDs, document IDs in order etc
const Engine * Engine::validate(const char * filename, const unsigned maxResults, const double k, const double b)
{
    // Check if the file has been opened successfully
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        std::cerr << Message[CANNOT_OPEN_FILE] << std::endl;
        return nullptr;
    }

    struct Util
    {
        unsigned cols;
        Util * next;

        Util() : cols(0), next(nullptr) {}
        ~Util() { if (next) delete next; }
    } * start = nullptr, ** current = &start;

    unsigned lines = 0;
    int currentID, previousID = -1;
    
    char docID[512];
    while (ifs >> docID)
    {
        // Read each document's ID and confirm it' s valid
        currentID = std::atoi(docID);
        if (currentID != previousID + 1)
        {
            std::cerr << Message[INVALID_ID_READ] << std::endl;
            std::cerr << lines << "| " << docID << "..." << std::endl;
            return nullptr;
        }

        *current = new Util();

        // Count the number of characters (non-consecutive whitespace)
        char ch;
        while (ifs.good())
        {
            if ((ch = (char) ifs.get()) == '\n')
                break;
            
            (*current)->cols++;
            if (std::isspace(ch))
            {
                while (ifs.good() && (ch = (char) ifs.peek()) != '\n' && std::isspace(ch))
                    ifs.get();
            
                if (ifs.eof() || (char) ifs.peek() == '\n')
                    (*current)->cols--;
            }
        }

        // If any line (i.e. document) is completely blank fail
        if (!(*current)->cols)
        {
            std::cerr << Message[EMPTY_DOC]  << " (" << currentID << ")" << std::endl;
            return nullptr;
        }

        lines++;

        previousID = currentID;
        current = &(*current)->next;
    }

    if (lines)
    {
        Info * const info = new Info(lines);

        unsigned id = 0;
        for (const Util * it = start; id < lines && it; id++, it = it->next)
            info->documents[id] = new char[info->columns[id] = it->cols];

        // Clean up memory and file before continuing
        delete start;
        ifs.clear(); ifs.seekg(std::ios::beg);

        return new Engine(ifs, info, (maxResults ? maxResults : 1), k, b);
    }

    return nullptr;
}

// Search Utility Functions:
double Engine::score(const unsigned id, const unsigned qsize, const char * q[], const PList * l[]) const
{
    const double d_over_avgdl = (double) info->words[id] / avgdl;
    
    double sum = 0.0;
    for (unsigned i = 0; i < qsize; i++)
    {
        const double IDF = l[i]->IDF, fq = (double) l[i]->instances[id];

        sum += IDF * ((fq * (k + 1.0)) / (fq + k * (1.0 - b + b * d_over_avgdl)));
    }

    return sum;
}

bool Engine::parseInput(char * input, unsigned& qsize, const char * q[], const PList * l[]) const
{
    const char del[] = " \t";

    unsigned i = 0;
    
    // If no input has been given fail
    if (!(q[i] = std::strtok(input, del)))
    {
        std::cerr << Message[NO_VALID_INPUT] << std::endl;
        return false;
    }
    
    // Ignore (invalid) queries that cannot be found within the trie
    // Get the Posting List of each valid query
    do
    {
        if (l[i] = trie.lookup(q[i]))
            i++;
        else
            std::cerr << Message[WORD_NOT_FOUND] << " (\"" << q[i] << "\")" << std::endl;
    } while (i < maxQueries && (q[i] = std::strtok(nullptr, del)));

    // In case all the queries were invalid fail
    if (i == 0)
    {
        std::cerr << Message[NO_VALID_INPUT] << std::endl;
        return false;
    }

    qsize = i;
    return true;
}

void Engine::printResult(const unsigned id, const double score, const unsigned i, const unsigned qsize, const char * q[]) const
{
    const unsigned len = info->columns[id];

    // Initialize the padding
    char * text[] = { new char[len], info->documents[id] };

    for (unsigned j = 0; j < len; j++)
        text[0][j] = ' ';

    // For each query locate every instance of it
    // in the specified document (id) and underline it
    for (unsigned j = 0; j < qsize; j++)
    {
        const char * substr = std::strstr(info->documents[id], q[j]);
        while (substr)
        {
            const unsigned beg = (unsigned) (substr - info->documents[id]);
            if (std::isspace(substr[std::strlen(q[j])]) && std::isspace(info->documents[id][beg - 1]))
            {
                const unsigned end = beg + std::strlen(q[j]) - 1;
                for (unsigned k = beg; k <= end; k++)
                    text[0][k] = '^';
            }

            substr = std::strstr(++substr, q[j]);
        }
    }

    // Print result info
    const unsigned spaces = 23;
    std::cout << std::setw(5) << i + 1 << '.'; // 6
    std::cout << '(' << std::setw(5) << id << ')'; // 7
    std::cout << '[' << std::setw(7) << std::setprecision(5) << std::showpos << score << "] " << std::noshowpos; // 10

    // Print Document and Padding
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    unsigned rowWidth = (len + spaces < w.ws_col ? len + spaces : w.ws_col), j = 0;
    
    // Print the first rowWidth - spaces characters of the document
    // on the same line as the result info
    for (unsigned l = 0; l < rowWidth - spaces; l++)
        std::cout << text[1][l];

    std::cout << std::endl;
    do
    {
        // Print Padding
        for (unsigned l = 0; l < spaces; l++)
            std::cout << ' ';
            
        for (unsigned l = j; l < j + rowWidth - spaces && l < len; l++)
            std::cout << text[0][l];

        std::cout << std::endl;

        j += rowWidth - spaces;

        // Print Document
        for (unsigned l = 0; l < spaces; l++)
            std::cout << ' ';
            
        for (unsigned l = j; l < j + rowWidth - spaces && l < len; l++)
            std::cout << text[1][l];

        std::cout << std::endl;
    } while (j < len);

    delete[] text[0];
}

// Search Engine Functionality:
void Engine::search(char * input) const
{
    const char  * q[maxQueries] = { nullptr };
    const PList * l[maxQueries] = { nullptr };
    unsigned qsize;
    
    // Parse input in order to retrieve separate valid queries
    // the corresponding Posting Lists
    if (!parseInput(input, qsize, q, l))
        return;

    struct Pair
    {
        unsigned id;
        double score;

        Pair() : id(0), score(0.0) {}

        bool operator>(const Pair& other) const { return (this->score > other.score); }
    } pair;

    heap<Pair> pairs(info->lines, heap<Pair>::greater);

    // Insert <document, score> pairs into heap
    // in order to find top maxResults documents
    for (unsigned id = 0; id < info->lines; id++)
    {
        unsigned j;
        for (j = 0; j < qsize; j++)
            if (l[j]->instances[id])
                break;

        if (j < qsize)
        {
            pair.id = id;
            pair.score = score(pair.id, qsize, q, l);
            pairs.push(pair);
        }
    }

    for (unsigned i = 0; i < maxResults && pairs.pop(pair); i++)
        printResult(pair.id, pair.score, i, qsize, q);
}

void Engine::docfreq() const
{
    trie.print();
}

void Engine::trmfreq(const int id, const char * word) const
{
    const PList * const pl = trie.lookup(word);

    if (pl)
        if (0 <= id && (unsigned) id <= info->lines - 1)
            std::cout << id << ' ' << word << ' ' << pl->instances[id] << std::endl;
        else
            std::cerr << Message[ID_OUT_OF_RANGE] << std::endl;
    else
        std::cerr << Message[WORD_NOT_FOUND] << std::endl;
}
