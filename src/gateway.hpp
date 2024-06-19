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
   public:
    APIGateway(const char* filename, int port);
    void start();
    void handle_filter_single_thread(const http_message::HttpRequest& request, http_message::HttpResponse& response);
    void handle_filter_multi_thread(const http_message::HttpRequest& request, http_message::HttpResponse& response);
    ~APIGateway();
};

APIGateway::APIGateway(const char* filename, int port)
    : http_server::HttpServer<http_message::HttpRequest, http_message::HttpResponse>(port), indexing::Indexing(filename) {
}

void APIGateway::start() {
    // Start the HTTP server
    HttpServer::start();
}

APIGateway::~APIGateway() {
    HttpServer::stop();
}

#endif  // API_GATEWAY_HPP
