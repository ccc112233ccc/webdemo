#include"Epoll.h"
/*
class Epoll {
private:
    static const int MAX_EVENTS_ = 100;
    int epoll_fd_;
    epoll_event events_[MAX_EVENTS_];

public:
    Epoll();
    ~Epoll();

    void add_fd(int fd, uint32_t op);
    std::vector<epoll_event> wait();
};
*/

Epoll::Epoll() {
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1) {
        perror("epoll_create");
        exit(1);
    }
}

Epoll::~Epoll() {
    close(epoll_fd_);
}

void Epoll::add_fd(int fd, uint32_t op) {
    epoll_event ev;
    ev.events = op;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl");
        exit(1);
    }
}

std::vector<epoll_event> Epoll::wait() {
    int nfds = epoll_wait(epoll_fd_, events_, MAX_EVENTS_, -1);
    if (nfds == -1) {
        perror("epoll_wait");
        exit(1);
    }
    std::vector<epoll_event> ret;
    for (int i = 0; i < nfds; i++) {
        ret.push_back(events_[i]);
    }
    return ret;
}