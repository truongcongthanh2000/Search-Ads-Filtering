#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>
#include <cstring>

namespace server {

class EpollEvent {
public:
    EpollEvent();
    void add(int fd, uint32_t events) const;
    void modify(int fd, uint32_t events) const;
    void remove(int fd) const;
    std::vector<struct epoll_event> wait();
    void close() const;

private:
    int epoll_fd_;
    static constexpr int max_events_ = SOMAXCONN;
    std::vector<struct epoll_event> events_;
};

} // namespace server
