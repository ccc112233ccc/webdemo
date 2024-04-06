#pragma once
#include <functional>

#include "Epoll.h"

class EventLoop {
 private:
  Epoll* ep_;
  std::function<void(EventLoop*)> timeout_callback_;

 public:
  EventLoop();
  ~EventLoop();
  void loop();
  // Epoll* get_epoll() { return &ep_; }
  void update_channel(Channel* channel);
  void set_timeout_callback(std::function<void(EventLoop*)> cb) {
    timeout_callback_ = cb;
  }
};