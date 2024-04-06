#include "TcpServer.h"

#include <functional>
#include <iostream>

#include "InetAddress.h"

TcpServer::TcpServer(int port) {
  acceptor_ = new Acceptor(&loop_, port);
  acceptor_->set_new_connection_callback(
      std::bind(&TcpServer::new_connect, this, std::placeholders::_1));
  loop_.set_timeout_callback(
      std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));
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
  conn->set_complete_callback(
      std::bind(&TcpServer::complete_callback, this, std::placeholders::_1));
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

void TcpServer::message_callback(Connection* conn, std::string message) {
  message = "hello, " + message;
  int len = message.size();
  std::string output_buffer((char*)&len, sizeof(len));
  output_buffer.append(message);
  // send(conn->fd(), output_buffer.data(), output_buffer.size(), 0);
  conn->send(output_buffer.data(), output_buffer.size());
}

void TcpServer::complete_callback(Connection* conn) {
  printf("消息发送完毕：%d, %s:%d\n", conn->fd(), conn->ip().c_str(),
         conn->port());
}

void TcpServer::epoll_timeout(EventLoop* loop) { printf("epoll 超时\n"); }