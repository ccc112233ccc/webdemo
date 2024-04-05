#pragma once
#include <functional>

#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

class Connection {
 private:
  EventLoop* loop_;
  Socket* clientsock_;
  Channel* clientch_;

  std::function<void(Connection*)> close_callback_;
  std::function<void(Connection*)> error_callback_;

 public:
  Connection(EventLoop* loop, Socket* clientsock);
  ~Connection();
  int fd() const { return clientsock_->fd(); }
  std::string ip() const { return clientsock_->ip(); }
  uint16_t port() const { return clientsock_->port(); }

  void close_callback();
  void error_callback();

  void set_close_callback(std::function<void(Connection*)> cb) {
    close_callback_ = cb;
  }
  void set_error_callback(std::function<void(Connection*)> cb) {
    error_callback_ = cb;
  }
};