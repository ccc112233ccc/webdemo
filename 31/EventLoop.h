#pragma once
#include <functional>
#include <memory>

#include "Epoll.h"

class EventLoop {
 private:
  std::unique_ptr<Epoll> ep_;
  std::function<void(EventLoop*)> timeout_callback_;

 public:
  EventLoop();
  ~EventLoop();
  void loop();
  void remove_channel(Channel* channel);
  void update_channel(Channel* channel);
  void set_timeout_callback(std::function<void(EventLoop*)> cb) {
    timeout_callback_ = cb;
  }
};