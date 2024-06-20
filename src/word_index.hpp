#ifndef WORD_INDEX_HPP
#define WORD_INDEX_HPP

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>

#include "tokenizer.hpp"
#include "utils.hpp"

class WordIndex {
   private:
    std::unordered_map<std::string, std::vector<int>> map_token_keywords;
    std::unordered_map<int, int> map_keyword_token_size;
    std::vector<std::string> keywords;

    std::vector<std::string> loadKeywords(const std::string& filename) {
        static const int limit = 1e7;
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::vector<std::string> keywords;
        std::string line;
        while (std::getline(infile, line)) {
            keywords.emplace_back(line);
        }
        while (keywords.size() < limit) {
            keywords.emplace_back(keywords[std::rand() % keywords.size()]);
        }
        return keywords;
    }

   public:
    WordIndex(const std::string& filename) {
        map_keyword_token_size.clear();
        map_token_keywords.clear();
        keywords.clear();

        std::cout << utils::time() << " Loading word index from file " << filename << std::endl;
        keywords = loadKeywords(filename);
        for (int idx = 0; idx < keywords.size(); idx++) {
            auto tokens = Tokenizer::tokenize(keywords[idx]);
            map_keyword_token_size[idx] = tokens.size();
            for (const auto& token : tokens) {
                map_token_keywords[token].emplace_back(idx);
            }
        }
        std::cout << utils::time() << " Load successfull" << std::endl;
    }

    const std::string& operator[](int idx) const {
        return keywords[idx];
    }

    bool contains(const std::string& token) const {
        return map_token_keywords.find(token) != map_token_keywords.end();
    }

    int get_token_size_by_keyword(int idx) const {
        auto it = map_keyword_token_size.find(idx);
        if (it == map_keyword_token_size.end()) {
            return 0;
        }
        return it->second;
    }

    int get_number_of_keywords() const {
        return keywords.size();
    }

    const std::vector<int>& get_keywords_by_token(const std::string& token) const {
        static const std::vector<int> empty_vector{};
        auto it = map_token_keywords.find(token);
        if (it == map_token_keywords.end()) {
            return empty_vector;
        }
        return it->second;
    }
};

#endif  // WORD_INDEX_HPP
