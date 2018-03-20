
#include "engine.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

int main(int argc, char * argv[])
{
    enum { FILE_FLAG = 1, FILE_INPUT, MAXQ_FLAG, MAXQ_INPUT };
    const char error[] = "<Error>: Unable to recognize arguement format";
    
    if (argc < 5)
    {
        std::cout << error << std::endl;
        return -1;
    }
    
    const int maxResults = std::atoi(argv[MAXQ_INPUT]);
    if (std::strcmp(argv[FILE_FLAG], "-i") || std::strcmp(argv[MAXQ_FLAG], "-k") || maxResults <= 0)
    {
        std::cout << error << std::endl;
        return -2;
    }

    const Engine * eng;
    if (!(eng = Engine::validate(argv[FILE_INPUT], (unsigned) maxResults)))
        return -3;

    // Comment out this line if it bothers your diff
    std::cout << "\n~ Welcome to Googolplex ~" << std::endl;

    char cmd[512]; const char del[] = " \t";
    do
    {
        // Comment out this line if it bothers your diff
        std::cout << ">: ";
        
        std::cin.getline(cmd, 512);

        if (!std::strncmp(cmd, "/search", 7))
        {
            eng->search(cmd + 7);
        }
        else if (!std::strcmp(cmd, "/df"))
        {
            eng->docfreq();
        }
        else if (!std::strncmp(cmd, "/tf", 3))
        {
            const char * tok[] = { std::strtok(cmd + 3, del), std::strtok(nullptr, del) };

            if (tok[0] && tok[1])
                eng->trmfreq(std::atoi(tok[1]), tok[0]);
        }
    } while (std::strcmp(cmd, "/exit"));

    delete eng;

    return 0;
}
