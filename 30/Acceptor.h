#pragma once
#include <functional>
#include <memory>

#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

class Acceptor {
 private:
  const std::unique_ptr<EventLoop>& loop_;
  Socket servsock_;
  Channel servch_;
  std::function<void(std::unique_ptr<Socket>)> new_connection_callback_;

 public:
  Acceptor(const std::unique_ptr<EventLoop>& loop, int port);
  ~Acceptor();
  void new_connect();
  void set_new_connection_callback(
      std::function<void(std::unique_ptr<Socket>)> cb) {
    new_connection_callback_ = cb;
  }
};