#include "Channel.h"

#include <iostream>

#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), inepoll_(false) {}

Channel::~Channel() {}

int Channel::fd() { return fd_; }

void Channel::useet() { events_ |= EPOLLET; }

void Channel::enable_reading() {
  events_ |= EPOLLIN;
  loop_->update_channel(this);
}

void Channel::disable_reading() {
  events_ &= ~EPOLLIN;
  loop_->update_channel(this);
}

void Channel::enable_writing() {
  events_ |= EPOLLOUT;
  loop_->update_channel(this);
}

void Channel::disable_writing() {
  events_ &= ~EPOLLOUT;
  loop_->update_channel(this);
}

void Channel::setin_epoll() { inepoll_ = true; }

void Channel::set_revents(uint32_t revents) { revents_ = revents; }

bool Channel::is_in_epoll() { return inepoll_; }

uint32_t Channel::events() { return events_; }

uint32_t Channel::revents() { return revents_; }

void Channel::handle_events() {
  if (revents_ & EPOLLRDHUP) {
    // 对端关闭连接
    // printf("对端关闭连接\n");
    // close(fd_);
    close_callback_();
  } else if (revents_ & (EPOLLIN | EPOLLPRI)) {
    read_callback_();
  } else if (revents_ & EPOLLOUT) {
    write_callback_();
  } else {
    // printf("未知事件\n");
    error_callback_();
  }
}
