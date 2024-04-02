#include"Channel.h"
#include"Epoll.h"

Channel::Channel(Epoll* epoll, int fd)
    : epoll_(epoll), fd_(fd), events_(0), revents_(0), inepoll_(false) {}

Channel::~Channel() {}

int Channel::fd() {
    return fd_;
}

void Channel::useet() {
    events_ |= EPOLLET;
}

void Channel::enable_reading() {
    events_ |= EPOLLIN;
    epoll_->update_channel(this);
}

void Channel::setin_epoll() {
    inepoll_ = true;
}

void Channel::set_revents(uint32_t revents) {
    revents_ = revents;
}

bool Channel::is_in_epoll() {
    return inepoll_;
}

uint32_t Channel::events() {
    return events_;
}

uint32_t Channel::revents() {
    return revents_;
}

