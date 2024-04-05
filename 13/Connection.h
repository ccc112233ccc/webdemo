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
};