#pragma once
#include "Epoll.h"

class EventLoop {
 private:
  Epoll ep_;

 public:
  EventLoop();
  ~EventLoop();
  void loop();
  Epoll* get_epoll() { return &ep_; }
};