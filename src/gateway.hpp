#ifndef API_GATEWAY_HPP
#define API_GATEWAY_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <functional>
#include "thread_pool.hpp"
#include "indexing.hpp"
#include "server/http_server.hpp"
#include "server/http_request.hpp"
#include "server/http_response.hpp"

class APIGateway : public http_server::HttpServer<http_message::HttpRequest, http_message::HttpResponse> {
public:
    APIGateway(size_t numThreads, const char* filename, int port);
    void start();
    ~APIGateway();
    
private:
    Indexing indexing;
};

APIGateway::APIGateway(size_t numThreads, const char* filename, int port)
    : http_server::HttpServer<http_message::HttpRequest, http_message::HttpResponse>(port), indexing(filename) {

}

void APIGateway::start() {
    // Start the HTTP server
    HttpServer::start();
}

APIGateway::~APIGateway() {
    HttpServer::stop();
}

#endif // API_GATEWAY_HPP
