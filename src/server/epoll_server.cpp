#include <csignal>

#include "epoll_server.hpp"

namespace server {

EpollServer::EpollServer(int port) :
        port_(port),
        max_workers_(std::thread::hardware_concurrency()),
        epoll_events_(max_workers_) {
    init();
}

EpollServer::~EpollServer() {
    if (!stop_) {
        stop();
    }
}

void EpollServer::init() {
    if ((server_fd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (::setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Failed to set socket options");
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_);

    if (::bind(server_fd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        std::cerr << "Failed to bind socket, errno: " << std::strerror(errno) << std::endl;
        throw std::runtime_error("Failed to bind socket");
    }

    ::listen(server_fd_, SOMAXCONN);

    for (int i = 0; i < max_workers_; ++i) {
        workers_.emplace_back(std::thread(&EpollServer::worker, this, i));
    }

    server_epoll_.add(server_fd_, EPOLLIN);
}
void EpollServer::worker(int worker_id) {
    // std::cout << "Worker thread started...\n";
    auto epoll = epoll_events_[worker_id];
    while (!stop_) {
        auto events = epoll.wait();
        for (const auto& event : events) {
            if (event.events & EPOLLIN) {
                char buffer[4096]; // TODO: handle large request
                ssize_t received_bytes = recv(event.data.fd, buffer, sizeof(buffer), 0);
                if (received_bytes <= 0) {
                    // Client close connection or error
                    epoll.remove(event.data.fd);
                    ::close(event.data.fd);
                    continue;
                }
                buffer[received_bytes] = '\0';
                handle_epollin(epoll, event.data.fd, std::move(std::string(buffer)));
            } else if (event.events & EPOLLOUT) {
                // handle_epollout(epoll, event.data.fd, std::move(std::string(buffer)));
            } else if (event.events & (EPOLLHUP | EPOLLERR)) {
                epoll.remove(event.data.fd);
                ::close(event.data.fd);
            } else {
                // std::cerr << "something unexpected\n";
            }
        }
    }
}

void EpollServer::start() {
    std::cout << "Server started listening on port: " << port_ << std::endl;
    serving_thread_ = std::thread(&EpollServer::serving, this);
}

void EpollServer::serving() {
    int worker_id = 0;
    while (!stop_) {
        auto events = server_epoll_.wait();
        for (const auto& event : events) {
            if (server_fd_ == event.data.fd) {
                int client_fd = ::accept(server_fd_, NULL, NULL);
                if (client_fd == -1) {
                    continue;
                }

                epoll_events_[worker_id].add(client_fd, EPOLLIN);
                worker_id++;
                if (worker_id == max_workers_) {
                    worker_id = 0;
                }
            }
        }
    }
}

void EpollServer::stop() {
    std::cout << "Stopping server..." << std::endl;
    stop_ = true;

    server_epoll_.close();
    serving_thread_.join();
    for (auto& epoll : epoll_events_) {
        epoll.close();
    }

    for (int i = 0; i < workers_.size(); ++i) {
        workers_[i].join();
    }
    std::cout << "Server stopped!" << std::endl;
}

} // namespace server