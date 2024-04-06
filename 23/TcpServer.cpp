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

  connections_[conn->fd()] = conn;

  if (new_connect_callback_) {
    new_connect_callback_(conn);
  }
}

void TcpServer::close_connect(Connection* conn) {
  if (close_connect_callback_) {
    close_connect_callback_(conn);
  }
  connections_.erase(conn->fd());
  delete conn;
}

void TcpServer::error_connect(Connection* conn) {
  if (error_connect_callback_) {
    error_connect_callback_(conn);
  }
  connections_.erase(conn->fd());
  delete conn;
}

void TcpServer::message_callback(Connection* conn, std::string message) {
  if (message_callback_) {
    message_callback_(conn, message);
  }
}

void TcpServer::complete_callback(Connection* conn) {
  if (complete_callback_) {
    complete_callback_(conn);
  }
}

void TcpServer::epoll_timeout(EventLoop* loop) {
  if (timeout_callback_) {
    timeout_callback_(&loop_);
  }
}