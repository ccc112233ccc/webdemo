#pragma once

#include <string>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
class TcpServer {
 private:
  EventLoop loop_;

 public:
  TcpServer(int port);
  TcpServer(const std::string& ip, int port);
  ~TcpServer();
  void start();
};