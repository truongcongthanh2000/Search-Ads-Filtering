#ifndef WORD_INDEX_HPP
#define WORD_INDEX_HPP

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "tokenizer.hpp"
#include "utils.hpp"

class WordIndex {
   public:
    WordIndex(const std::string& filename) {
        map_keyword_tokens_size.clear();
        map_tokens_keyword.clear();
        keywords.clear();

        std::cout << utils::time() << " Loading word index from file " << filename << std::endl;
        keywords = loadKeywords(filename);
        for (int idx = 0; idx < keywords.size(); idx++) {
            auto tokens = Tokenizer::tokenize(keywords[idx]);
            map_keyword_tokens_size[idx] = tokens.size();
            for (const auto& token : tokens) {
                map_tokens_keyword[token].emplace_back(idx);
            }
        }
        std::cout << utils::time() << " Load successfull" << std::endl;
    }

    auto find

   private:
    std::unordered_map<std::string, std::vector<int>> map_tokens_keyword;
    std::unordered_map<int, int> map_keyword_tokens_size;
    std::vector<std::string> keywords;

    std::vector<std::string>& loadKeywords(const std::string& filename) {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::vector<std::string> keywords;
        std::string line;
        while (std::getline(infile, line)) {
            keywords.emplace_back(line);
        }
        return keywords;
    }
};

#endif  // WORD_INDEX_HPP
