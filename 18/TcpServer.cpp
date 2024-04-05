#include "TcpServer.h"

#include <functional>
#include <iostream>

#include "InetAddress.h"

TcpServer::TcpServer(int port) {
  acceptor_ = new Acceptor(&loop_, port);
  acceptor_->set_new_connection_callback(
      std::bind(&TcpServer::new_connect, this, std::placeholders::_1));
}

void TcpServer::start() { loop_.loop(); }

TcpServer::~TcpServer() {
  delete acceptor_;
  for (auto& conn : connections_) {
    delete conn.second;
  }
}

void TcpServer::new_connect(Socket* clientsock) {
  Connection* conn = new Connection(&loop_, clientsock);
  conn->set_close_callback(
      std::bind(&TcpServer::close_connect, this, std::placeholders::_1));
  conn->set_error_callback(
      std::bind(&TcpServer::error_connect, this, std::placeholders::_1));
  printf("新连接：%d, %s:%d\n", conn->fd(), conn->ip().c_str(), conn->port());

  connections_[conn->fd()] = conn;
}

void TcpServer::close_connect(Connection* conn) {
  printf("连接关闭：%d, %s:%d\n", conn->fd(), conn->ip().c_str(), conn->port());
  connections_.erase(conn->fd());
  delete conn;
}

void TcpServer::error_connect(Connection* conn) {
  printf("连接错误：%d, %s:%d\n", conn->fd(), conn->ip().c_str(), conn->port());
  connections_.erase(conn->fd());
  delete conn;
}