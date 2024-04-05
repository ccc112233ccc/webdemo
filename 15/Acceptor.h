#pragma once
#include <functional>

#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

class Acceptor {
 private:
  EventLoop* loop_;
  Socket* servsock_;
  Channel* servch_;
  std::function<void(Socket*)> new_connection_callback_;

 public:
  Acceptor(EventLoop* loop, int port);
  ~Acceptor();
  void new_connect();
  void set_new_connection_callback(const std::function<void(Socket*)>& cb) {
    new_connection_callback_ = cb;
  }
};