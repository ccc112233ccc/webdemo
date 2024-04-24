#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>

#include <vector>

class Channel;
class Epoll {
 private:
  static const int MAX_EVENTS_ = 100;
  int epoll_fd_;
  epoll_event events_[MAX_EVENTS_];

 public:
  Epoll();
  ~Epoll();

  void add_fd(int fd, uint32_t op);
  void update_channel(Channel* channel);
  void remove_channel(Channel* channel);

  // 参数表示超时时间
  std::vector<Channel*> wait(int timeout = -1);
};