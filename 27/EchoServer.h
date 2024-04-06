#pragma once
#include "Connection.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "ThreadPool.h"

class EchoServer {
 private:
  TcpServer server_;
  ThreadPool* threadpoll_;

 public:
  EchoServer(uint16_t port, int threadnum = 4, int workerthreadnum = 5);
  ~EchoServer();

  void start();
  void HandleNewConnection(Connection* conn);
  void HandleCloseConnection(Connection* conn);
  void HandleErrorConnection(Connection* conn);

  void HandleMessage(Connection* conn, std::string& message);
  void HandleComplete(Connection* conn);
  void HandleTimeout(EventLoop* loop);
  void OnMessage(Connection* conn, std::string& message);
};