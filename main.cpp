#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "word_index.hpp"
#include "thread_pool.hpp"

using namespace std;

vector<string> Filter(const WordIndex& wordIndex, const vector<string>& tokens_query) {
    unordered_map<string, int> count;

    for (const auto& token : tokens_query) {
        auto it = wordIndex.map_tokens_keyword.find(token);
        if (it != wordIndex.map_tokens_keyword.end()) {
            for (const auto& keyword : it->second) {
                count[keyword]++;
            }
        }
    }

    vector<string> ans;
    for (const auto& [keyword, cnt] : count) {
        if (cnt >= wordIndex.map_keyword_tokens_size.at(keyword)) {
            ans.push_back(keyword);
        }
    }

    return ans;
}

class APIGateway {
public:
    APIGateway(size_t numThreads, const string& filename);
    vector<string> filter(const vector<string>& tokens_query);

private:
    ThreadPool threadPool;
    WordIndex wordIndex;
};

APIGateway::APIGateway(size_t numThreads, const string& filename)
    : threadPool(numThreads), wordIndex(filename) {}

vector<string> APIGateway::filter(const vector<string>& tokens_query) {
    auto task = [this, tokens_query]() {
        lock_guard<mutex> lock(map_mutex);
        return Filter(wordIndex, tokens_query);
    };
    future<vector<string>> result = threadPool.enqueue(task);
    return result.get();
}

int main() {
    // Example setup: assuming 'keywords.txt' is the file containing the keywords and tokens
    APIGateway apiGateway(4, "keywords.txt");

    vector<string> tokens_query = {"token1", "token2"};
    vector<string> result = apiGateway.filter(tokens_query);

    for (const auto& keyword : result) {
        cout << keyword << endl;
    }

    return 0;
}
