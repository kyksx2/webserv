#include "Config.hpp"

Config::Config() {
    //
}

Config::Config(const std::string& filepath) {
    parse(filepath);
}

// std::string readFile(const std::string& filename)
// {
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         throw std::runtime_error("Impossible d'ouvrir le fichier");
//     }
    
//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     return buffer.str();
// }

void    Config::parse(const std::string& filepath)
{

    std::ifstream infile(filepath);
    std::string line;
    std::string copy_file;

    while (std::getline(infile, line))
        copy_file += line + '\n';
    
    // std::cout << copy_file << std::endl;
    infile.close();
}

