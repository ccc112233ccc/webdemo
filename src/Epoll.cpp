#include "Epoll.h"

#include <iostream>

#include "Channel.h"
#include "Tracer.h"

Epoll::Epoll() {
  epoll_fd_ = epoll_create(1);
  if (epoll_fd_ == -1) {
    perror("epoll_create");
    exit(1);
  }
}

Epoll::~Epoll() { close(epoll_fd_); }

void Epoll::update_channel(Channel* channel) {
  epoll_event ev;
  ev.events = channel->events();
  ev.data.ptr = channel;
  if (channel->is_in_epoll()) {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, channel->fd(), &ev) == -1) {
      perror("epoll_ctl");
      exit(1);
    }
    return;
  } else {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &ev) == -1) {
      perror("epoll_ctl");
      exit(1);
    }
    channel->setin_epoll();
  }
}

void Epoll::remove_channel(Channel* channel) {
  if (channel->is_in_epoll()) {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, channel->fd(), NULL) == -1) {
      perror("epoll_ctl");
      exit(1);
    }
  }
}

std::vector<Channel*> Epoll::wait(int timeout) {
  int nfds = epoll_wait(epoll_fd_, events_, MAX_EVENTS_, timeout);
  if (nfds == -1) {
    perror("epoll_wait");
    exit(1);
  }
  if (nfds == 0) {
    // 超时
    return std::vector<Channel*>();
  }
  std::vector<Channel*> ret;
  for (int i = 0; i < nfds; i++) {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->set_revents(events_[i].events);
    ret.push_back(channel);
  }
  return ret;
}