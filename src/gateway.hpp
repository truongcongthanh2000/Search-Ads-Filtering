#ifndef API_GATEWAY_HPP
#define API_GATEWAY_HPP

#include <cstring>
#include <functional>
#include <iostream>
#include <map>

#include "indexing.hpp"
#include "server/http_request.hpp"
#include "server/http_response.hpp"
#include "server/http_server.hpp"
#include "thread_pool.hpp"

class APIGateway : public http_server::HttpServer<http_message::HttpRequest, http_message::HttpResponse>, indexing::Indexing {
private:
    int number_threads, number_tasks;  // used in threadpool
public:
    APIGateway(const char* filename, int port, int number_threads, int number_tasks);
    void start();
    void handle_filter_single_thread(const http_message::HttpRequest& request, http_message::HttpResponse& response);
    void handle_filter_multi_thread(const http_message::HttpRequest& request, http_message::HttpResponse& response);
    ~APIGateway();
    int get_number_threads() {
        return number_threads;
    }
    int get_number_tasks() {
        return number_tasks;
    }
};

APIGateway::APIGateway(const char* filename, int port, int number_threads, int number_tasks)
    : http_server::HttpServer<http_message::HttpRequest, http_message::HttpResponse>(port), indexing::Indexing(filename), number_threads(number_threads), number_tasks(number_tasks) {
}

void APIGateway::start() {
    // Start the HTTP server
    HttpServer::start();
}

APIGateway::~APIGateway() {
    HttpServer::stop();
}

#endif  // API_GATEWAY_HPP
