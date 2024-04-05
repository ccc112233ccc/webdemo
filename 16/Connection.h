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

 public:
  Connection(EventLoop* loop, Socket* clientsock);
  ~Connection();
  int fd() const { return clientsock_->fd(); }
  std::string ip() const { return clientsock_->ip(); }
  uint16_t port() const { return clientsock_->port(); }
};