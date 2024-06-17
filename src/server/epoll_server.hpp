#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

#include "epoll_event.hpp"

namespace server {

class EpollServer {
public:
    EpollServer(int port);
    virtual ~EpollServer();

    void start();
    void stop();

    virtual void handle_epollin(EpollEvent& epoll, int client_fd, std::string&& buffer) = 0;
    virtual void handle_epollout(EpollEvent& epoll, int client_fd, std::string&& buffer) = 0;

private:
    void init();
    void worker(int worker_id);
    void serving();

    const int max_workers_;
    int server_fd_;
    int port_;

    std::atomic_bool stop_{false};
    std::vector<EpollEvent> epoll_events_;
    EpollEvent server_epoll_;
    std::vector<std::thread> workers_;
    std::thread serving_thread_;
};

} // namespace server
