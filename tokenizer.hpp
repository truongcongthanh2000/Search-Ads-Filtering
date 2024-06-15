#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>
#include <sstream>

struct Tokenizer {
    static std::vector<std::string> tokenize(const std::string& str) {
        std::vector<std::string> tokens;
        std::istringstream iss(str);
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

#endif // TOKENIZER_HPP
