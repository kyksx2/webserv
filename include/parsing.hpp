#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

std::string cleanContent(const std::string& fileContent);
std::string readFile(const std::string& filename);
enum TokenType		determineType(const std::string& word);
std::vector<Token> tokeniseContent(const std::string& fileContent);

#endif