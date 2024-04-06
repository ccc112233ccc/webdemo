#include "Epoll.h"

#include "Channel.h"

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
std::vector<Channel*> Epoll::wait() {
  int nfds = epoll_wait(epoll_fd_, events_, MAX_EVENTS_, -1);
  if (nfds == -1) {
    perror("epoll_wait");
    exit(1);
  }
  std::vector<Channel*> ret;
  for (int i = 0; i < nfds; i++) {
    Channel* channel = (Channel*)events_[i].data.ptr;
    channel->set_revents(events_[i].events);
    ret.push_back(channel);
  }
  return ret;
}