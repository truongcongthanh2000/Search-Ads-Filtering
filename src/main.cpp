#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>

#include "gateway.hpp"
#include "thread_pool.hpp"
#include "word_index.hpp"

/* List all keyword has the index in the range [start, end) that pass filtering with the search query
    Consider n keywords in range [start, end)
    Split all n keywords to sqrt(n) blocks, each block will contains sqrt(n) keywords
    Traverse each block and use a cursor to traverse only keyword in the current block for each token
    Use array 1D with size sqrt(n) to counting the number of tokens in query that keyword contains
        and use this array to check if the keyword has been counting all tokens
*/
std::vector<std::string> Filter(const WordIndex& word_index, const std::string& query, int start, int end) {
    auto tokens = Tokenizer::tokenize(query);
    int tokens_size = tokens.size();
    std::vector<int> last_position_keyword(tokens_size, 0);

    auto remove_token = [&tokens, &tokens_size, &last_position_keyword](int idx) {
        // remove tokens[idx] in O(1)
        std::swap(tokens[idx], tokens[tokens_size - 1]);
        std::swap(last_position_keyword[idx], last_position_keyword[tokens_size - 1]);
        tokens.pop_back();
        last_position_keyword.pop_back();
        tokens_size--;
    };

    // remove all token that wordIndex does not contains, or list keyword outside with range [start, end)
    for (int idx = tokens_size - 1; idx >= 0; idx--) {
        if (!word_index.contains(tokens[idx])) {
            remove_token(idx);
            continue;
        }
        /* find the first position in keywords has index >= start
            and check case list keyword of tokens[idx] is outside with range [start, end) or not
        */
        auto& keywords = word_index.get_keywords_by_token(tokens[idx]);
        auto first_position = std::lower_bound(keywords.begin(), keywords.end(), start);
        if (first_position == keywords.end() || *first_position >= end) {
            remove_token(idx);
        } else {
            last_position_keyword[idx] = first_position - keywords.begin();  // move pointer into block inside range [start, end)
        }
    }

    // check corner case tokens is empty after remove
    if (tokens.empty()) {
        static std::vector<std::string> empty_vector{};
        return empty_vector;
    }

    // Split all n keywords to sqrt(n) blocks, each block will contains sqrt(n) keywords
    int n = end - start;
    int number_of_blocks = int(sqrt(n));
    int block_size = (n + number_of_blocks - 1) / number_of_blocks;  // = ceil(n / number_of_blocks)
    std::vector<int> count(block_size, 0);

    std::vector<std::string> ans;
    int current_idx_keyword = start;
    // Loop until tokens_size = 0 or traverse all n keywords
    while (tokens_size > 0 && current_idx_keyword < end) {
        int R = std::min(current_idx_keyword + block_size, end);
        for (int idx = tokens_size - 1; idx >= 0; idx--) {
            auto& keywords = word_index.get_keywords_by_token(tokens[idx]);

            // Traverse only keyword in the current block
            int& position = last_position_keyword[idx];
            while (position < keywords.size() && keywords[position] < R) {
                // convert index keyword to index of array count for counting
                int idx_count = keywords[position] - current_idx_keyword;
                count[idx_count]++;
                position++;
            }

            // check remove current token if we traverse all keywords or exceed range [start, end)
            if (position == keywords.size() || keywords[position] >= end) {
                remove_token(idx);
            }
        }

        // list all keywords that has been counting all tokens
        for (int idx = 0; idx < R - current_idx_keyword; idx++) {
            // convert index counting to index keyword
            int idx_keyword = idx + current_idx_keyword;
            if (count[idx] == word_index.get_token_size_by_keyword(idx_keyword)) {
                ans.push_back(word_index[idx_keyword]);
            }
            count[idx] = 0;  // reset counting for used in the next block
        }

        current_idx_keyword = R;
    }
    return ans;
}

std::string format_json(const std::string& query, const std::vector<std::string>& keywords) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"query\": " << "\"" << query << "\",\n";
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

void APIGateway::handle_filter_single_thread(const http_message::HttpRequest& request, http_message::HttpResponse& response) {
    std::cout << utils::time() << " Start handle_filter_single_thread " << request.get_uri_raw() << std::endl;
    auto start_time = std::chrono::system_clock::now();

    // Extract user_query from request
    std::string query = request["query"];
    const auto word_index = getWordIndex();
    auto keywords = Filter(*word_index, query, 0, word_index->get_number_of_keywords());
    response.set_header("Content-Type", "application/json");
    response.set_body(format_json(query, keywords));
    response.set_status(http_message::HttpStatus::OK);

    auto end_time = std::chrono::system_clock::now();
    std::cout << utils::time() << " Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
}

void APIGateway::handle_filter_multi_thread(const http_message::HttpRequest& request, http_message::HttpResponse& response) {
    std::cout << utils::time() << " Start handle_filter_multi_thread " << request.get_uri_raw() << std::endl;
    auto start_time = std::chrono::system_clock::now();

    // Extract user_query from request
    std::string query = request["query"];
    const std::shared_ptr<WordIndex> word_index = getWordIndex();
    std::vector<std::future<std::vector<std::string>>> results;
    // Use threadpool with 4 threads and split n keywords to 100 tasks, each task will process (n/100) keywords
    int nkeywords = word_index->get_number_of_keywords();
    int ntasks = get_number_tasks();
    int nkeywords_per_task = (nkeywords + ntasks - 1) / ntasks;

    try {
        ThreadPool pool(get_number_threads());
        for (int start = 0; start < nkeywords;) {
            int end = std::min(start + nkeywords_per_task, nkeywords);
            results.emplace_back(pool.enqueue([word_index, query, start, end] {
                return Filter(*word_index, query, start, end);
            }));
            start = end;
        }

        std::vector<std::string> keywords;
        for (auto& result : results) {
            auto keywords_future = result.get();
            keywords.insert(keywords.end(), keywords_future.begin(), keywords_future.end());
        }
        response.set_header("Content-Type", "application/json");
        response.set_body(format_json(query, keywords));
        response.set_status(http_message::HttpStatus::OK);
    } catch (const std::exception& e) {
        std::cout << utils::time() << " handle_filter_multi_thread error: " << e.what() << std::endl;
        response.set_header("Content-Type", "application/json");
        response.set_body(e.what());
        response.set_status(http_message::HttpStatus::InternalServerError);
    }
    auto end_time = std::chrono::system_clock::now();
    std::cout << utils::time() << " Execution time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << " milliseconds" << std::endl;
}

int main(int argc, char *argv[]) {
    int number_threads = std::stoi(argv[1]);
    int number_tasks = std::stoi(argv[2]);
    // Example setup: assuming 'keywords.txt' is the file containing the keywords and tokens
    APIGateway apiGateway("keywords.txt", 8080, number_threads, number_tasks);
    apiGateway.register_handler("/filter_single_thread/", [&apiGateway](const http_message::HttpRequest& request, http_message::HttpResponse& response) {
        apiGateway.handle_filter_single_thread(request, response);
    });

    apiGateway.register_handler("/filter_multi_thread/", [&apiGateway](const http_message::HttpRequest& request, http_message::HttpResponse& response) {
        apiGateway.handle_filter_multi_thread(request, response);
    });

    apiGateway.start();
    std::cout << "Enter [quit] to graceful shutdown the server!" << std::endl;

    std::string cmd;
    while (std::cin >> cmd, cmd != "quit") {
    }
    return 0;
}
