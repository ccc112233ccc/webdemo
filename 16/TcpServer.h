#pragma once

#include <map>
#include <string>

#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
class TcpServer {
 private:
  EventLoop loop_;  // 一个TcpServer对象对应多个EventLoop对象，现在只有一个
  Acceptor* acceptor_;  // 一个TcpServer对象对应一个Acceptor对象
  std::map<int, Connection*> connections_;

 public:
  TcpServer(int port);
  TcpServer(const std::string& ip, int port);
  ~TcpServer();
  void start();
  void new_connect(Socket* clientsock);
};