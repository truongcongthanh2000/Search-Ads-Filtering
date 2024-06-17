#include <stdexcept>

#include "epoll_event.hpp"

namespace server {

EpollEvent::EpollEvent() {
    epoll_fd_ = ::epoll_create1(0);
    if (epoll_fd_ == -1) {
        throw std::runtime_error("Failed to create epoll");
    }
    events_.resize(max_events_);
}

void EpollEvent::add(int fd, uint32_t events) const {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0) {
        throw std::runtime_error("Failed to add fd to epoll");
    }
}

void EpollEvent::modify(int fd, uint32_t events) const {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event) < 0) {
        throw std::runtime_error("Failed to modify fd to epoll");
    }
}

void EpollEvent::remove(int fd) const {
    if (::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
        throw std::runtime_error("Failed to remove fd to epoll: " + std::to_string(errno));
    }
}

std::vector<struct epoll_event> EpollEvent::wait() {
    int num_events = ::epoll_wait(epoll_fd_, events_.data(), max_events_, 100);
    std::vector<struct epoll_event> result(events_.begin(), events_.begin() + num_events);
    return std::move(result);
}

void EpollEvent::close() const {
    ::close(epoll_fd_);
}

} // namespace server
