#pragma once
#include "Connection.h"
#include "EventLoop.h"
#include "TcpServer.h"

class EchoServer {
 private:
  TcpServer server_;

 public:
  EchoServer(uint16_t port);
  ~EchoServer();

  void start();
  void HandleNewConnection(Connection* conn);
  void HandleCloseConnection(Connection* conn);
  void HandleErrorConnection(Connection* conn);

  void HandleMessage(Connection* conn, std::string& message);
  void HandleComplete(Connection* conn);

  void HandleTimeout(EventLoop* loop);
};