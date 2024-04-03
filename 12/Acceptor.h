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

 public:
  Acceptor(EventLoop* loop, int port);
  ~Acceptor();
};