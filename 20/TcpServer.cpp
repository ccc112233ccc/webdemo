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
  conn->set_message_callback(std::bind(&TcpServer::message_callback, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
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

void TcpServer::message_callback(Connection* conn, std::string& message) {
  message = "hello, " + message;
  int len = message.size();
  std::string output_buffer(sizeof(len), 0);
  memcpy(&output_buffer[0], &len, sizeof(len));
  output_buffer.append(message);
  send(conn->fd(), output_buffer.data(), output_buffer.size(), 0);
}