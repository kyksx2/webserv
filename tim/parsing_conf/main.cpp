#include "Config.hpp"

int main(int ac, char **av)
{
    std::string const file_path(av[1]);
    if (ac == 2)
        Config config(file_path);
    // else if (ac == 1)
    //     Config config();
}
