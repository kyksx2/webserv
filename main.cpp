#include "main.hpp"
#include <string.h>

void    parse(char *conf_file)
{
    std::ifstream infile(conf_file);
    std::string line;
    std::string copy_file;

    while (std::getline(infile, line))
        copy_file += line + '\n';
    std::cout << copy_file << std::endl;
    infile.close();
}

int main(int ac, char **av)
{
    Config *config;

    if (ac == 2)
    {
        parse(av[1]);
    }
    else
        exit(0);
}
