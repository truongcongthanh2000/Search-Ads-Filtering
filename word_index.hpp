#ifndef WORD_INDEX_HPP
#define WORD_INDEX_HPP

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include "tokenizer.hpp"

class WordIndex {
public:
    std::unordered_map<std::string, std::vector<std::string>> map_tokens_keyword;
    std::unordered_map<std::string, int> map_keyword_tokens_size;

    WordIndex(const std::string& filename) {
        loadFromFile(filename);
    }

private:
    void loadFromFile(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::string line;
        while (std::getline(infile, line)) {
            auto tokens = Tokenizer::tokenize(line);
            if (!tokens.empty()) {
                std::string keyword = tokens[0];
                tokens.erase(tokens.begin()); // Remove the keyword from the list of tokens
                map_keyword_tokens_size[keyword] = tokens.size();
                for (const auto& token : tokens) {
                    map_tokens_keyword[token].push_back(keyword);
                }
            }
        }
    }
};

#endif // WORD_INDEX_HPP
