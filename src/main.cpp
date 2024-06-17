#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "word_index.hpp"
#include "thread_pool.hpp"
#include "gateway.hpp"

std::vector<std::string> Filter(const WordIndex& wordIndex, const std::vector<std::string>& tokens_query) {
    std::unordered_map<std::string, int> count;

    for (const auto& token : tokens_query) {
        auto it = wordIndex.map_tokens_keyword.find(token);
        if (it != wordIndex.map_tokens_keyword.end()) {
            for (const auto& keyword : it->second) {
                count[keyword]++;
            }
        }
    }

    std::vector<std::string> ans;
    for (const auto& [keyword, cnt] : count) {
        if (cnt >= wordIndex.map_keyword_tokens_size.at(keyword)) {
            ans.push_back(keyword);
        }
    }

    return ans;
}

std::string format_json(const std::string& query, const std::vector<std::string>& keywords) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"query\": " << query << ",\n";
    oss << "  \"length\": " << keywords.size() << ",\n";
    oss << "  \"keyword\": [";
    for (size_t i = 0; i < keywords.size(); ++i) {
        oss << "\"" << keywords[i] << "\"";
        if (i < keywords.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]\n";
    oss << "}";
    return oss.str();
}

void handle_filter(const http_message::HttpRequest& request, http_message::HttpResponse& response) {
    // Extract user_query from request
    std::string query = request["query"];
    response.set_header("Content-Type", "application/json");
    response.set_body(format_json(query, {query}));
    response.set_status(http_message::HttpStatus::OK);
}

int main() {
    // Example setup: assuming 'keywords.txt' is the file containing the keywords and tokens
    APIGateway apiGateway(4, "keywords.txt", 8080);
    apiGateway.register_handler("/filter/", handle_filter);

    apiGateway.start();
    std::cout << "Enter [quit] to graceful shutdown the server!" << std::endl;

    std::string cmd;
    while (std::cin >> cmd, cmd != "quit") {}
    return 0;
}
