#pragma once
#include "Connection.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "ThreadPool.h"

class EchoServer {
 private:
  TcpServer server_;
  ThreadPool threadpoll_;

 public:
  EchoServer(uint16_t port, int threadnum = 4, int workerthreadnum = 5);
  ~EchoServer();

  void start();
  void stop();
  void HandleNewConnection(spConnection conn);
  void HandleCloseConnection(spConnection conn);
  void HandleErrorConnection(spConnection conn);

  void HandleMessage(spConnection conn, std::string& message);
  void HandleComplete(spConnection conn);
  void HandleTimeout(EventLoop* loop);
  void OnMessage(spConnection conn, std::string& message);
};