#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#pragma once

#include <functional>

#include "epoll_server.hpp"
#include "http_request.hpp"

namespace http_server {

static inline bool match_path(const std::string& pattern, const std::string& path) {
    if (pattern.back() == '*') {
        return path.size() >= pattern.size() - 1 &&
            std::equal(pattern.begin(), pattern.end() - 1, path.begin());
    } else {
        if (pattern.back() == '/' && path.size() == pattern.size() - 1) {
            return std::equal(pattern.begin(), pattern.end() - 1, path.begin());
        } else {
            return path == pattern;
        }
    }
}

template <typename RequestType, typename ResponseType>
class HttpServer : public server::EpollServer {
public:
    using HandlerFunc = std::function<void(const RequestType&, ResponseType&)>;

    HttpServer(int port) : server::EpollServer(port) {
    }

    void register_handler(const std::string& pattern, const HandlerFunc handler) {
        handlers_[pattern] = handler;
    }

    void handle_epollin(server::EpollEvent& epoll, int client_fd, std::string&& buffer) final {
        ResponseType response;
        bool should_close = true;
        try {
            RequestType request(std::move(buffer));
            auto it = find_handler(request.get_uri());
            if (it != handlers_.end()) {
                it->second(request, response);
            } else {
                response.set_status(http_message::HttpStatus::NotFound);
                response.set_body("NOT FOUND");
            }
            
            // close connection with HTTP/1.0 version
            if (request.get_version() != http_message::HttpVersion::HTTP_1_0) {
                should_close = false;
            }

        }  catch (const std::invalid_argument &e) {
            response.set_status(http_message::HttpStatus::BadRequest);
            response.set_body(e.what());
        } catch (const std::exception &e) {
            response.set_status(http_message::HttpStatus::InternalServerError);
            response.set_body(e.what());
        }

        send_response(epoll, client_fd, response);

        if (should_close) {
            epoll.remove(client_fd);
            ::close(client_fd);
        }
    }

    void handle_epollout(server::EpollEvent& epoll, int client_fd, std::string&& buffer) final {
    }

private:
    auto find_handler(const std::string& uri) const {
        for (auto it = handlers_.begin(); it != handlers_.end(); it++) {
            if (match_path(it->first, uri)) {
                return it;
            }
        }
        return handlers_.end();
    }

    // TODO: send response via epoll EPOLLOUT?
    void send_response(server::EpollEvent& epoll, int client_fd, const ResponseType& response) {
        auto response_str = response.to_string();
        auto response_len = response_str.length();

        while (response_len > 0) {
            auto sent_bytes = send(client_fd, response_str.c_str(), response_str.length(), 0);
            if (sent_bytes < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // retry
                    continue;
                }
                epoll.remove(client_fd);
                ::close(client_fd);
                return;
            }
            response_len -= sent_bytes;
        }
    }

    std::map<std::string, HandlerFunc> handlers_;
};

} // namespace http_server

#endif // HTTP_SERVER_HPP